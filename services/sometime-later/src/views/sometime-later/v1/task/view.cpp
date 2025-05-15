#include "view.hpp"

#include <userver/components/component.hpp>

#include "docs/yaml/api/api.hpp"

namespace views::sometime_later::v1::task::post {

SometimeLaterV1TaskPost::SometimeLaterV1TaskPost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::sometime_later::handlers::CreateTaskRequest>(config, component_context),
      tasks_manager_(component_context.FindComponent<::sometime_later::contract::managers::TasksManager>()) {}

views::contract::models::ApiResponse SometimeLaterV1TaskPost::
    Handle(::sometime_later::handlers::CreateTaskRequest&& request, userver::server::request::RequestContext&&) const {
    tasks_manager_.CreateTask(std::move(request));

    return contract::models::ApiResponseFactory::Created();
}

}  // namespace views::sometime_later::v1::task::post
