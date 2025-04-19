#include "../contract/models/responses.hpp"

#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value_builder.hpp>

namespace views::contract::models {

std::string ApiResponse::ToJson() const {
    return std::visit([](const auto& response) -> std::string {
        using T = std::decay_t<decltype(response)>;
        
        if constexpr (std::is_same_v<T, EmptyResponse>) {
            return "{}";
        } 
        else if constexpr (std::is_same_v<T, SuccessResponse<userver::formats::json::Value>>) {
            return userver::formats::json::ToString(response.data);
        }
        else if constexpr (std::is_same_v<T, ErrorResponse>) {
            userver::formats::json::ValueBuilder builder;
            builder["message"] = response.error.message;
            builder["code"] = response.error.code;
            return userver::formats::json::ToString(builder.ExtractValue());
        }
        else {
            static_assert(false, "Unsupported response type");
            return "{}";
        }
    }, response_);
}

} // namespace views::contract::models
