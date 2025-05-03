#pragma once

#include <string>
#include <type_traits>

#include <userver/clients/http/client.hpp>
#include <userver/clients/http/response.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/logging/log.hpp>
#include <userver/tracing/span.hpp>
#include <userver/utils/fmt_compat.hpp>

#include "docs/yaml/definitions.hpp"
#include "models/exceptions.hpp"

namespace clients::contract {

namespace impl {

template <typename T, typename = void>
struct HasAsMethod : std::false_type {};

template <typename T>
struct HasAsMethod<T, std::void_t<decltype(std::declval<T>().template As<userver::formats::json::Value>())>>
    : std::true_type {};

}  // namespace impl

class BaseClient {
public:
    BaseClient(userver::clients::http::Client& http_client, std::string base_url)
        : http_client_(http_client), base_url_(std::move(base_url)) {}

    virtual ~BaseClient() = default;

protected:
    // Обобщенный метод для выполнения запросов
    template <typename ResponseType, typename RequestType = userver::formats::json::Value>
    ResponseType PerformRequest(
        userver::clients::http::HttpMethod method,
        const std::string& path,
        const std::optional<RequestType>& request_body = std::nullopt,
        const userver::clients::http::Headers& headers = {},
        std::chrono::milliseconds timeout = std::chrono::seconds{5}
    ) const {
        userver::tracing::Span span(fmt::format("http_client_{}", GetClientName()));  // Имя для трейсинга
        span.AddTag("http.url", base_url_ + path);
        span.AddTag("http.method", userver::clients::http::ToString(method));

        std::string body_str;
        if (request_body) {
            userver::formats::json::Value json_body;
            // Проверяем, есть ли у RequestType метод As<JsonValue> (codegen структура)
            if constexpr (impl::HasAsMethod<RequestType>::value) {
                json_body = userver::formats::json::ValueBuilder(*request_body).ExtractValue();
            } else if constexpr (std::is_same_v<RequestType, userver::formats::json::Value>) {
                json_body = *request_body;
            } else {
                // Попытка сериализовать стандартными средствами (если поддерживается)
                // Можно добавить статическую проверку или кастомную сериализацию, если нужно
                json_body = userver::formats::json::ValueBuilder(*request_body).ExtractValue();
            }
            body_str = userver::formats::json::ToString(json_body);
            span.AddTag("http.request_body_size", std::to_string(body_str.length()));
        }

        auto http_request =
            http_client_.CreateRequest(method, base_url_ + path).headers(headers).timeout(timeout).data(body_str);

        // Устанавливаем Content-Type если есть тело запроса
        if (!body_str.empty()) {
            http_request.headers({{"Content-Type", "application/json"}});
        }

        LOG_INFO() << "Performing HTTP request: " << userver::clients::http::ToString(method) << " "
                   << base_url_ + path;

        auto response_ptr = http_request.perform();
        span.AddTag("http.status_code", std::to_string(response_ptr->StatusCodeInt()));
        span.AddTag("http.response_body_size", std::to_string(response_ptr->body().length()));

        LOG_INFO() << "Received HTTP response: Status " << response_ptr->StatusCodeInt();

        // Обработка ответа
        try {
            return ParseResponse<ResponseType>(*response_ptr);
        } catch (const models::HttpClientException& ex) {
            LOG_WARNING() << "HTTP Client Error (" << ex.GetStatusCode() << "): " << ex.what()
                          << ", URL: " << base_url_ + path;
            if (ex.GetErrorBody()) {
                LOG_WARNING() << "Error body: code=" << ex.GetErrorBody()->code
                              << ", message=" << ex.GetErrorBody()->message;
            }
            throw;  // Перебрасываем исключение дальше
        } catch (const std::exception& ex) {
            LOG_ERROR() << "Failed to parse response or unexpected client error: " << ex.what()
                        << ", URL: " << base_url_ + path << ", Status: " << response_ptr->StatusCodeInt()
                        << ", Body: " << response_ptr->body();
            throw models::ClientException(
                fmt::format("Failed to handle response from {}: {}", base_url_ + path, ex.what())
            );
        }
    }

    // Метод для парсинга ответа и обработки ошибок HTTP
    template <typename ResponseType>
    ResponseType ParseResponse(const userver::clients::http::Response& response) const {
        const auto status_code = response.status_code();
        const auto& body = response.body();

        if (status_code < userver::clients::http::Status::kBadRequest) {  // 2xx
            if constexpr (std::is_void_v<ResponseType>) {
                // Если ожидаемый тип ответа void, просто возвращаем
                return;
            } else {
                // Пытаемся распарсить тело как ResponseType
                try {
                    if (body.empty() && !std::is_same_v<ResponseType, userver::formats::json::Value>) {
                        // Если тело пустое, а ожидается не json::Value, возможно, это ошибка
                        // или просто пустой успешный ответ. Для простоты создаем пустой объект.
                        // Либо можно бросать исключение, если пустой ответ недопустим.
                        if constexpr (std::is_default_constructible_v<ResponseType>) {
                            LOG_DEBUG() << "Received empty body for non-JSON response type, returning default "
                                           "constructed object.";
                            return ResponseType{};
                        } else {
                            throw models::ClientException(
                                "Received empty response body, but expected non-default-constructible type"
                            );
                        }
                    }
                    auto json_response = userver::formats::json::FromString(body);
                    return json_response.As<ResponseType>();
                } catch (const std::exception& e) {
                    throw models::ClientException(fmt::format(
                        "Failed to parse {} response body: {}", userver::utils::TypeName<ResponseType>(), e.what()
                    ));
                }
            }
        } else {  // 4xx, 5xx
            // Пытаемся распарсить тело как стандартную ошибку
            std::optional<sometime_later::handlers::Error> error_body;
            try {
                if (!body.empty()) {
                    error_body = userver::formats::json::FromString(body).As<sometime_later::handlers::Error>();
                }
            } catch (const std::exception& e) {
                LOG_WARNING() << "Failed to parse error response body: " << e.what()
                              << ", Status: " << response.StatusCodeInt() << ", Body: " << body;
                // Оставляем error_body = std::nullopt
            }

            // Бросаем конкретное исключение по коду статуса
            std::string error_message =
                error_body ? error_body->message : fmt::format("HTTP error {}", response.StatusCodeInt());

            switch (status_code) {
                case userver::clients::http::Status::kBadRequest:
                    throw models::BadRequestError(std::move(error_body));
                case userver::clients::http::Status::kNotFound:
                    throw models::NotFoundError(std::move(error_body));
                case userver::clients::http::Status::kInternalServerError:
                    throw models::InternalServerError(std::move(error_body));
                default:
                    throw models::HttpClientException(error_message, status_code, std::move(error_body));
            }
        }
    }

    // Виртуальный метод для получения имени клиента (для логов/трейсов)
    virtual std::string GetClientName() const = 0;

private:
    userver::clients::http::Client& http_client_;
    std::string base_url_;
};

}  // namespace clients::contract
