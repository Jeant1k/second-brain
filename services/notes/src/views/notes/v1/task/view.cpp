#include "view.hpp"

#include <userver/components/component.hpp>

#include "docs/yaml/api.hpp"

namespace views::notes::v1::task::post {

NotesV1TaskPost::NotesV1TaskPost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::notes::handlers::CreateTaskRequest>(config, component_context),
      tasks_manager_(component_context.FindComponent<::notes::contract::managers::TasksManager>()) {}

views::contract::models::ApiResponse
NotesV1TaskPost::Handle(::notes::handlers::CreateTaskRequest&& request, userver::server::request::RequestContext&&)
    const {
    tasks_manager_.CreateTask(std::move(request));

    return contract::models::ApiResponseFactory::Created();
}

}  // namespace views::notes::v1::task::post
