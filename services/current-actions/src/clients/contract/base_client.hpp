#pragma once

#include <string>
#include <type_traits>

#include <userver/clients/http/client.hpp>
#include <userver/clients/http/response.hpp>
#include <userver/compiler/demangle.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/logging/log.hpp>
#include <userver/tracing/span.hpp>
#include <userver/utils/fmt_compat.hpp>

#include "docs/yaml/clients/sometime_later/definitions.hpp"
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
    template <typename ResponseType, typename RequestType = userver::formats::json::Value>
    ResponseType PerformRequest(
        userver::clients::http::HttpMethod method,
        const std::string& path,
        const std::optional<RequestType>& request_body = std::nullopt,
        const userver::clients::http::Headers& headers = {},
        std::chrono::milliseconds timeout = std::chrono::seconds{5}
    ) const {
        std::string body_str;
        if (request_body.has_value()) {
            userver::formats::json::Value json_body;
            if constexpr (impl::HasAsMethod<RequestType>::value) {
                json_body = userver::formats::json::ValueBuilder(request_body.value()).ExtractValue();
            } else if constexpr (std::is_same_v<RequestType, userver::formats::json::Value>) {
                json_body = request_body.value();
            } else {
                json_body = userver::formats::json::ValueBuilder(request_body.value()).ExtractValue();
            }
            body_str = userver::formats::json::ToString(json_body);
        }

        auto http_request = http_client_.CreateRequest()
                                .method(method)
                                .url(base_url_ + path)
                                .headers(headers)
                                .timeout(timeout)
                                .data(body_str);

        if (!body_str.empty()) {
            http_request.headers({{"Content-Type", "application/json"}});
        }

        LOG_INFO() << "Performing HTTP request: " << std::string{userver::clients::http::ToStringView(method)} << " "
                   << base_url_ + path;

        auto response_ptr = http_request.perform();

        LOG_INFO() << "Received HTTP response: Status " << ToString(response_ptr->status_code());

        try {
            return ParseResponse<ResponseType>(*response_ptr);
        } catch (const models::HttpClientException& ex) {
            LOG_WARNING() << "HTTP Client Error (" << ex.GetStatusCode() << "): " << ex.what()
                          << ", URL: " << base_url_ + path;
            if (ex.GetErrorBody().has_value()) {
                LOG_WARNING() << "Error body: code=" << ex.GetErrorBody().value().code
                              << ", message=" << ex.GetErrorBody().value().message;
            }
            throw;
        } catch (const std::exception& ex) {
            LOG_ERROR() << "Failed to parse response or unexpected client error: " << ex.what()
                        << ", URL: " << base_url_ + path << ", Status: " << ToString(response_ptr->status_code())
                        << ", Body: " << response_ptr->body();
            throw models::ClientException(
                fmt::format("Failed to handle response from {}: {}", base_url_ + path, ex.what())
            );
        }
    }

    template <typename ResponseType>
    ResponseType ParseResponse(const userver::clients::http::Response& response) const {
        const auto status_code = response.status_code();
        const auto& body = response.body();

        if (status_code < userver::clients::http::Status::kBadRequest) {
            if constexpr (std::is_void_v<ResponseType>) {
                return;
            } else {
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
                        "Failed to parse {} response body: {}", userver::compiler::GetTypeName<ResponseType>(), e.what()
                    ));
                }
            }
        } else {
            std::optional<sometime_later::handlers::Error> error_body;
            try {
                if (!body.empty()) {
                    error_body = userver::formats::json::FromString(body).As<sometime_later::handlers::Error>();
                }
            } catch (const std::exception& e) {
                LOG_WARNING() << "Failed to parse error response body: " << e.what()
                              << ", Status: " << ToString(response.status_code()) << ", Body: " << body;
            }

            std::string error_message =
                error_body ? error_body->message : fmt::format("HTTP error {}", ToString(response.status_code()));

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

private:
    userver::clients::http::Client& http_client_;
    std::string base_url_;
};

}  // namespace clients::contract