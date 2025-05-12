#include "view.hpp"

#include <userver/components/component.hpp>

#include "../../../../../waiting/contract/models/exceptions.hpp"
#include "docs/yaml/api.hpp"

namespace views::waiting::v1::task::remove::post {

WaitingV1TaskRemovePost::WaitingV1TaskRemovePost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::waiting::handlers::TaskIdRequest>(config, component_context),
      tasks_manager_(component_context.FindComponent<::waiting::contract::managers::TasksManager>()) {}

views::contract::models::ApiResponse WaitingV1TaskRemovePost::
    Handle(::waiting::handlers::TaskIdRequest&& request, userver::server::request::RequestContext&&) const {
    try {
        tasks_manager_.DeleteTask(std::move(request));
    } catch (const ::waiting::contract::models::TaskNotFoundException& ex) {
        return contract::models::ApiResponseFactory::NotFound(
            fmt::format("An error occurred while processing the request: {}", ex.what())
        );
    }

    return contract::models::ApiResponseFactory::Ok();
}

}  // namespace views::waiting::v1::task::remove::post
