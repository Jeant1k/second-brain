#include "view.hpp"

#include <userver/components/component.hpp>

#include "../../../../../notes/contract/models/exceptions.hpp"
#include "docs/yaml/api.hpp"

namespace views::notes::v1::task::complete::post {

NotesV1TaskCompletePost::NotesV1TaskCompletePost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::notes::handlers::TaskIdRequest>(config, component_context),
      tasks_manager_(component_context.FindComponent<::notes::contract::managers::TasksManager>()) {}

views::contract::models::ApiResponse
NotesV1TaskCompletePost::Handle(::notes::handlers::TaskIdRequest&& request, userver::server::request::RequestContext&&)
    const {
    try {
        tasks_manager_.CompleteTask(std::move(request));
    } catch (const ::notes::contract::models::TaskNotFoundException& ex) {
        return contract::models::ApiResponseFactory::NotFound(
            fmt::format("An error occurred while processing the request: {}", ex.what())
        );
    }

    return contract::models::ApiResponseFactory::Ok();
}

}  // namespace views::notes::v1::task::complete::post
