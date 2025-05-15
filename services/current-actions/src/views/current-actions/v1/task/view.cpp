#include "view.hpp"

#include <userver/components/component.hpp>

#include "docs/yaml/api/api.hpp"

namespace views::current_actions::v1::task::post {

CurrentActionsV1TaskPost::CurrentActionsV1TaskPost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::current_actions::handlers::CreateTaskRequest>(config, component_context),
      tasks_manager_(component_context.FindComponent<::current_actions::contract::managers::TasksManager>()) {}

views::contract::models::ApiResponse CurrentActionsV1TaskPost::
    Handle(::current_actions::handlers::CreateTaskRequest&& request, userver::server::request::RequestContext&&) const {
    tasks_manager_.CreateTask(std::move(request));

    return contract::models::ApiResponseFactory::Created();
}

}  // namespace views::current_actions::v1::task::post
