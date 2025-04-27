#include "view.hpp"

#include <userver/components/component.hpp>

#include "docs/yaml/api.hpp"

#include "../../../../../current_actions/contract/models/exceptions.hpp"

namespace views::current_actions::v1::task::update::post {

CurrentActionsV1TaskUpdatePost::CurrentActionsV1TaskUpdatePost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::current_actions::handlers::UpdateTaskRequest>(config, component_context),
      tasks_manager_(component_context.FindComponent<::current_actions::contract::managers::TasksManager>()) {}

views::contract::models::ApiResponse CurrentActionsV1TaskUpdatePost::
    Handle(::current_actions::handlers::UpdateTaskRequest&& request, userver::server::request::RequestContext&&) const {
    try {
        tasks_manager_.UpdateTask(std::move(request));
    } catch (const ::current_actions::contract::models::NoFieldsProvidedException& ex) {
        return contract::models::ApiResponseFactory::BadRequest(
            fmt::format("An error occurred while processing the request: {}", ex.what())
        );
    } catch (const ::current_actions::contract::models::TaskNotFoundException& ex) {
        return contract::models::ApiResponseFactory::NotFound(
            fmt::format("An error occurred while processing the request: {}", ex.what())
        );
    }

    return contract::models::ApiResponseFactory::Accepted();
}

}  // namespace views::current_actions::v1::task::update::post
