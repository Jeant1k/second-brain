#include "view.hpp"

#include <userver/components/component.hpp>

#include "../../../../../../sometime_later/contract/models/exceptions.hpp"

namespace views::internal::sometime_later::v1::task::move::post {

InternalSometimeLaterV1TaskMovePost::InternalSometimeLaterV1TaskMovePost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::sometime_later::handlers::MoveTaskRequest>(config, component_context),
      tasks_manager_(component_context.FindComponent<::sometime_later::contract::managers::TasksManager>()) {}

views::contract::models::ApiResponse InternalSometimeLaterV1TaskMovePost::
    Handle(::sometime_later::handlers::MoveTaskRequest&& request, userver::server::request::RequestContext&&) const {
    try {
        tasks_manager_.MoveTask(std::move(request));
    } catch (const ::sometime_later::contract::models::WrongUserIdException& ex) {
        return contract::models::ApiResponseFactory::BadRequest(
            fmt::format("An error occurred while processing the request: {}", ex.what())
        );
    }

    return contract::models::ApiResponseFactory::Ok();
}

}  // namespace views::internal::sometime_later::v1::task::move::post
