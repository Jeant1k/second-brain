#pragma once

#include <optional>
#include <stdexcept>
#include <string>

#include <userver/clients/http/response.hpp>
#include <userver/formats/json/value.hpp>

#include "docs/yaml/definitions.hpp"

namespace clients::contract::models {

class ClientException : public std::runtime_error {
public:
    explicit ClientException(const std::string& message) : std::runtime_error(message) {}
};

class HttpClientException : public ClientException {
public:
    HttpClientException(
        const std::string& message,
        userver::clients::http::Status status_code,
        std::optional<sometime_later::handlers::Error>&& error_body = std::nullopt
    )
        : ClientException(message), status_code_(status_code), error_body_(std::move(error_body)) {}

    userver::clients::http::Status GetStatusCode() const { return status_code_; }
    const std::optional<sometime_later::handlers::Error>& GetErrorBody() const { return error_body_; }

private:
    userver::clients::http::Status status_code_;
    std::optional<sometime_later::handlers::Error> error_body_;
};

class BadRequestError : public HttpClientException {
public:
    explicit BadRequestError(std::optional<sometime_later::handlers::Error>&& error_body = std::nullopt)
        : HttpClientException("Bad Request", userver::clients::http::Status::kBadRequest, std::move(error_body)) {}
};

class NotFoundError : public HttpClientException {
public:
    explicit NotFoundError(std::optional<sometime_later::handlers::Error>&& error_body = std::nullopt)
        : HttpClientException("Not Found", userver::clients::http::Status::kNotFound, std::move(error_body)) {}
};

class InternalServerError : public HttpClientException {
public:
    explicit InternalServerError(std::optional<sometime_later::handlers::Error>&& error_body = std::nullopt)
        : HttpClientException(
              "Internal Server Error",
              userver::clients::http::Status::kInternalServerError,
              std::move(error_body)
          ) {}
};

}  // namespace clients::contract::models
