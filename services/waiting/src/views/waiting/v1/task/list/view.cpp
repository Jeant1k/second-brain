#include "view.hpp"

#include <userver/components/component.hpp>

#include "docs/yaml/api.hpp"

namespace views::waiting::v1::task::list::post {

WaitingV1TaskListPost::WaitingV1TaskListPost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::waiting::handlers::ListTasksRequest>(config, component_context),
      tasks_manager_(component_context.FindComponent<::waiting::contract::managers::TasksManager>()) {}

views::contract::models::ApiResponse WaitingV1TaskListPost::
    Handle(::waiting::handlers::ListTasksRequest&& request, userver::server::request::RequestContext&&) const {
    return contract::models::ApiResponseFactory::Ok(tasks_manager_.ListTasks(std::move(request)));
}

}  // namespace views::waiting::v1::task::list::post
