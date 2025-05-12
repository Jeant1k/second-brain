#include "view.hpp"

#include <userver/components/component.hpp>

#include "../../../../../../waiting/contract/models/exceptions.hpp"

namespace views::internal::waiting::v1::task::move::post {

InternalWaitingV1TaskMovePost::InternalWaitingV1TaskMovePost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::waiting::handlers::MoveTaskRequest>(config, component_context),
      tasks_manager_(component_context.FindComponent<::waiting::contract::managers::TasksManager>()) {}

views::contract::models::ApiResponse InternalWaitingV1TaskMovePost::
    Handle(::waiting::handlers::MoveTaskRequest&& request, userver::server::request::RequestContext&&) const {
    try {
        tasks_manager_.MoveTask(std::move(request));
    } catch (const ::waiting::contract::models::WrongUserIdException& ex) {
        return contract::models::ApiResponseFactory::BadRequest(
            fmt::format("An error occurred while processing the request: {}", ex.what())
        );
    }

    return contract::models::ApiResponseFactory::Ok();
}

}  // namespace views::internal::waiting::v1::task::move::post
