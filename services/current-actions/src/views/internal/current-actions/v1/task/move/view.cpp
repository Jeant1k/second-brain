#include "view.hpp"

#include <userver/components/component.hpp>

namespace views::internal::current_actions::v1::task::move::post {

InternalCurrentActionsV1TaskMovePost::InternalCurrentActionsV1TaskMovePost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::current_actions::handlers::MoveTaskRequest>(config, component_context),
      tasks_manager_(component_context.FindComponent<::current_actions::contract::managers::TasksManager>()) {}

views::contract::models::ApiResponse InternalCurrentActionsV1TaskMovePost::
    Handle(::current_actions::handlers::MoveTaskRequest&& request, userver::server::request::RequestContext&&) const {
    tasks_manager_.MoveTask(std::move(request));

    return contract::models::ApiResponseFactory::Ok();
}

}  // namespace views::internal::current_actions::v1::task::move::post
