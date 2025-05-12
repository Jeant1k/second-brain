#include "view.hpp"

#include <userver/components/component.hpp>

#include "docs/yaml/api.hpp"

#include "../../../../../waiting/contract/models/exceptions.hpp"

namespace views::waiting::v1::task::update::post {

WaitingV1TaskUpdatePost::WaitingV1TaskUpdatePost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::waiting::handlers::UpdateTaskRequest>(config, component_context),
      tasks_manager_(component_context.FindComponent<::waiting::contract::managers::TasksManager>()) {}

views::contract::models::ApiResponse WaitingV1TaskUpdatePost::
    Handle(::waiting::handlers::UpdateTaskRequest&& request, userver::server::request::RequestContext&&) const {
    try {
        tasks_manager_.UpdateTask(std::move(request));
    } catch (const ::waiting::contract::models::NoFieldsProvidedException& ex) {
        return contract::models::ApiResponseFactory::BadRequest(
            fmt::format("An error occurred while processing the request: {}", ex.what())
        );
    } catch (const ::waiting::contract::models::TaskNotFoundException& ex) {
        return contract::models::ApiResponseFactory::NotFound(
            fmt::format("An error occurred while processing the request: {}", ex.what())
        );
    }

    return contract::models::ApiResponseFactory::Accepted();
}

}  // namespace views::waiting::v1::task::update::post
