#include "view.hpp"

#include <userver/components/component.hpp>

#include "docs/yaml/api.hpp"

namespace views::notes::v1::task::list::post {

NotesV1TaskListPost::NotesV1TaskListPost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::notes::handlers::ListTasksRequest>(config, component_context),
      tasks_manager_(component_context.FindComponent<::notes::contract::managers::TasksManager>()) {}

views::contract::models::ApiResponse
NotesV1TaskListPost::Handle(::notes::handlers::ListTasksRequest&& request, userver::server::request::RequestContext&&)
    const {
    return contract::models::ApiResponseFactory::Ok(tasks_manager_.ListTasks(std::move(request)));
}

}  // namespace views::notes::v1::task::list::post
