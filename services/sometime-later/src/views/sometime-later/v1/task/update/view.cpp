#include "view.hpp"

#include <userver/components/component.hpp>

#include "docs/yaml/api.hpp"

#include "../../../../../sometime_later/contract/models/exceptions.hpp"

namespace views::sometime_later::v1::task::update::post {

SometimeLaterV1TaskUpdatePost::SometimeLaterV1TaskUpdatePost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::sometime_later::handlers::UpdateTaskRequest>(config, component_context),
      tasks_manager_(component_context.FindComponent<::sometime_later::contract::managers::TasksManager>()) {}

views::contract::models::ApiResponse SometimeLaterV1TaskUpdatePost::
    Handle(::sometime_later::handlers::UpdateTaskRequest&& request, userver::server::request::RequestContext&&) const {
    try {
        tasks_manager_.UpdateTask(std::move(request));
    } catch (const ::sometime_later::contract::models::NoFieldsProvidedException& ex) {
        return contract::models::ApiResponseFactory::BadRequest(
            fmt::format("An error occurred while processing the request: {}", ex.what())
        );
    } catch (const ::sometime_later::contract::models::TaskNotFoundException& ex) {
        return contract::models::ApiResponseFactory::NotFound(
            fmt::format("An error occurred while processing the request: {}", ex.what())
        );
    }

    return contract::models::ApiResponseFactory::Accepted();
}

}  // namespace views::sometime_later::v1::task::update::post
