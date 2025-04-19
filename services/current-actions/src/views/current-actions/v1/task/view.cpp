#include "view.hpp"

#include <userver/components/component.hpp>

#include "docs/yaml/api.hpp"

namespace views::current_actions::v1::task::post {

namespace {

using contract::models::ApiResponseFactory;
using ::current_actions::contract::managers::TasksManager;
using ::current_actions::handlers::CreateTaskRequest;
using views::contract::models::ApiResponse;

}  // namespace

CurrentActionsV1TaskPost::CurrentActionsV1TaskPost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<CreateTaskRequest>(config, component_context),
      tasks_manager_(component_context.FindComponent<TasksManager>()) {}

ApiResponse CurrentActionsV1TaskPost::Handle(CreateTaskRequest&& request, userver::server::request::RequestContext&&)
    const {
    tasks_manager_.CreateTask(std::move(request));

    return ApiResponseFactory::Created();
}

}  // namespace views::current_actions::v1::task::post
