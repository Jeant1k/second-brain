#pragma once

#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/server/request/request_context.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/logging/log.hpp>

#include "models/responses.hpp"

namespace views::contract {

template <typename RequestType = userver::formats::json::Value>
class BaseHandler : public userver::server::handlers::HttpHandlerBase {
public:
    using HttpHandlerBase::HttpHandlerBase;

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context) const override {
        
        auto& response = request.GetHttpResponse();
        response.SetContentType(userver::http::content_type::kApplicationJson);
        
        RequestType parsed_request;
        try {
            if constexpr (std::is_same_v<RequestType, userver::formats::json::Value>) {
                parsed_request = userver::formats::json::FromString(request.RequestBody());
            } else {
                auto request_json = userver::formats::json::FromString(request.RequestBody());
                parsed_request = request_json.As<RequestType>();
            }
        } catch (const std::exception& ex) {
            LOG_INFO() << "Failed to parse request: " << ex.what();
            auto error_response = models::ApiResponseFactory::BadRequest(
                std::string("Invalid request format: ") + ex.what());
            response.SetStatus(static_cast<userver::server::http::HttpStatus>(error_response.GetStatusCode()));
            return error_response.ToJson();
        }
        
        auto api_response = Handle(std::move(parsed_request), std::move(context));
        
        response.SetStatus(static_cast<userver::server::http::HttpStatus>(api_response.GetStatusCode()));
        return api_response.ToJson();
    }

protected:
    virtual models::ApiResponse Handle(
        RequestType&& request,
        userver::server::request::RequestContext&& context) const = 0;
};

} // namespace views::contract
