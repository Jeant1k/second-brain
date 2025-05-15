#include "view.hpp"

#include <userver/components/component.hpp>

#include "docs/yaml/api/api.hpp"

namespace views::current_actions::v1::task::list::post {

CurrentActionsV1TaskListPost::CurrentActionsV1TaskListPost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::current_actions::handlers::ListTasksRequest>(config, component_context),
      tasks_manager_(component_context.FindComponent<::current_actions::contract::managers::TasksManager>()) {}

views::contract::models::ApiResponse CurrentActionsV1TaskListPost::
    Handle(::current_actions::handlers::ListTasksRequest&& request, userver::server::request::RequestContext&&) const {
    return contract::models::ApiResponseFactory::Ok(tasks_manager_.ListTasks(std::move(request)));
}

}  // namespace views::current_actions::v1::task::list::post
