#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>

namespace views::current_actions::v1::task::post {

class CurrentActionsV1TaskPost final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "current-actions-v1-task-post";

    CurrentActionsV1TaskPost(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context);

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext&
    ) const override;

private:

};
       
}  // namespace views::current_actions::v1::task::post
