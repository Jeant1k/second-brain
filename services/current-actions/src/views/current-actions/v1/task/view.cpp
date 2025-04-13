#include "view.hpp"

#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/utils/assert.hpp>

#include "docs/yaml/api.hpp"

namespace views::current_actions::v1::task::post {

namespace {

using ::current_actions::contract::managers::TasksManager;
using ::current_actions::handlers::CreateTaskRequest;

}

CurrentActionsV1TaskPost::CurrentActionsV1TaskPost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
) : userver::server::handlers::HttpHandlerBase(config, component_context),
    tasks_manager_(component_context.FindComponent<TasksManager>()) {}

std::string CurrentActionsV1TaskPost::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&
) const {
    request.GetHttpResponse().SetContentType(userver::http::content_type::kApplicationJson);
 
    auto request_json = userver::formats::json::FromString(request.RequestBody());

    auto create_task_request = request_json.As<CreateTaskRequest>();

    tasks_manager_.CreateTask(std::move(create_task_request));

    auto& response = request.GetHttpResponse();
    response.SetContentType(userver::http::content_type::kApplicationJson);
    response.SetStatus(userver::server::http::HttpStatus::kCreated);
    
    return "";
}

}  // namespace views::current_actions::v1::task::post
