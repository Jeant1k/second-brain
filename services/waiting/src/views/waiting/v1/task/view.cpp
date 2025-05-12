#include "view.hpp"

#include <userver/components/component.hpp>

#include "docs/yaml/api.hpp"

namespace views::waiting::v1::task::post {

WaitingV1TaskPost::WaitingV1TaskPost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::waiting::handlers::CreateTaskRequest>(config, component_context),
      tasks_manager_(component_context.FindComponent<::waiting::contract::managers::TasksManager>()) {}

views::contract::models::ApiResponse
WaitingV1TaskPost::Handle(::waiting::handlers::CreateTaskRequest&& request, userver::server::request::RequestContext&&)
    const {
    tasks_manager_.CreateTask(std::move(request));

    return contract::models::ApiResponseFactory::Created();
}

}  // namespace views::waiting::v1::task::post
