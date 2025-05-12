#include "view.hpp"

#include <userver/components/component.hpp>

#include "docs/yaml/api.hpp"

#include "../../../../../notes/contract/models/exceptions.hpp"

namespace views::notes::v1::task::update::post {

NotesV1TaskUpdatePost::NotesV1TaskUpdatePost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::notes::handlers::UpdateTaskRequest>(config, component_context),
      tasks_manager_(component_context.FindComponent<::notes::contract::managers::TasksManager>()) {}

views::contract::models::ApiResponse NotesV1TaskUpdatePost::
    Handle(::notes::handlers::UpdateTaskRequest&& request, userver::server::request::RequestContext&&) const {
    try {
        tasks_manager_.UpdateTask(std::move(request));
    } catch (const ::notes::contract::models::NoFieldsProvidedException& ex) {
        return contract::models::ApiResponseFactory::BadRequest(
            fmt::format("An error occurred while processing the request: {}", ex.what())
        );
    } catch (const ::notes::contract::models::TaskNotFoundException& ex) {
        return contract::models::ApiResponseFactory::NotFound(
            fmt::format("An error occurred while processing the request: {}", ex.what())
        );
    }

    return contract::models::ApiResponseFactory::Accepted();
}

}  // namespace views::notes::v1::task::update::post
