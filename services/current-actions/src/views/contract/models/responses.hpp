#pragma once

#include <string>
#include <userver/formats/json/value_builder.hpp>
#include <variant>

#include "docs/yaml/definitions.hpp"

namespace views::contract::models {

template <typename T>
struct SuccessResponse {
    T data{};
};

struct EmptyResponse {};

struct ErrorResponse {
    current_actions::handlers::Error error{};
};

class ApiResponse {
public:
    using ResponseVariant = std::variant<EmptyResponse, SuccessResponse<userver::formats::json::Value>, ErrorResponse>;

    explicit ApiResponse(ResponseVariant&& response, const int status_code)
        : response_(std::move(response)), status_code_(status_code) {}

    int GetStatusCode() const { return status_code_; }
    const ResponseVariant& GetResponse() const { return response_; }

    std::string ToJson() const;

private:
    const ResponseVariant response_;
    const int status_code_;
};

class ApiResponseFactory {
public:
    static ApiResponse Ok() { return ApiResponse(EmptyResponse{}, 200); }

    template <typename T>
    static ApiResponse Ok(T&& data) {
        SuccessResponse<userver::formats::json::Value> response{
            userver::formats::json::ValueBuilder(std::forward<T>(data)).ExtractValue()
        };
        return ApiResponse(std::move(response), 200);
    }

    static ApiResponse Created() { return ApiResponse(EmptyResponse{}, 201); }

    static ApiResponse Accepted() { return ApiResponse(EmptyResponse{}, 202); }

    static ApiResponse BadRequest(const std::string& message, const std::string& code = "BAD_REQUEST") {
        ErrorResponse error{current_actions::handlers::Error{message, code}};
        return ApiResponse(std::move(error), 400);
    }

    static ApiResponse NotFound(const std::string& message, const std::string& code = "NOT_FOUND") {
        ErrorResponse error{current_actions::handlers::Error{message, code}};
        return ApiResponse(std::move(error), 404);
    }

    static ApiResponse InternalError(const std::string& message, const std::string& code = "INTERNAL_ERROR") {
        ErrorResponse error{current_actions::handlers::Error{message, code}};
        return ApiResponse(std::move(error), 500);
    }
};

}  // namespace views::contract::models
