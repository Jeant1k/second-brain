#include "view.hpp"

#include <userver/components/component.hpp>

#include "docs/yaml/api.hpp"

namespace views::current_actions::v1::task::list::post {

namespace {

using contract::models::ApiResponseFactory;
using ::current_actions::contract::managers::TasksManager;
using ::current_actions::handlers::ListTasksRequest;
using views::contract::models::ApiResponse;

}  // namespace

CurrentActionsV1TaskListPost::CurrentActionsV1TaskListPost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<ListTasksRequest>(config, component_context),
      tasks_manager_(component_context.FindComponent<TasksManager>()) {}

ApiResponse
CurrentActionsV1TaskListPost::Handle(ListTasksRequest&& request, userver::server::request::RequestContext&&) const {
    auto tasks = tasks_manager_.ListTasks(std::move(request));
}

}  // namespace views::current_actions::v1::task::list::post
