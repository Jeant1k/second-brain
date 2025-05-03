#include "view.hpp"

#include <userver/components/component.hpp>

#include "docs/yaml/api.hpp"

namespace views::sometime_later::v1::task::list::post {

SometimeLaterV1TaskListPost::SometimeLaterV1TaskListPost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::sometime_later::handlers::ListTasksRequest>(config, component_context),
      tasks_manager_(component_context.FindComponent<::sometime_later::contract::managers::TasksManager>()) {}

views::contract::models::ApiResponse SometimeLaterV1TaskListPost::
    Handle(::sometime_later::handlers::ListTasksRequest&& request, userver::server::request::RequestContext&&) const {
    return contract::models::ApiResponseFactory::Ok(tasks_manager_.ListTasks(std::move(request)));
}

}  // namespace views::sometime_later::v1::task::list::post
