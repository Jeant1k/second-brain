#include "view.hpp"

#include <userver/components/component.hpp>

#include "docs/yaml/api/api.hpp"

#include "../../../../../current_actions/contract/models/exceptions.hpp"

namespace views::current_actions::v1::task::reactivate::post {

CurrentActionsV1TaskReactivatePost::CurrentActionsV1TaskReactivatePost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::current_actions::handlers::TaskIdRequest>(config, component_context),
      tasks_manager_(component_context.FindComponent<::current_actions::contract::managers::TasksManager>()) {}

views::contract::models::ApiResponse CurrentActionsV1TaskReactivatePost::
    Handle(::current_actions::handlers::TaskIdRequest&& request, userver::server::request::RequestContext&&) const {
    try {
        tasks_manager_.ReactivateTask(std::move(request));
    } catch (const ::current_actions::contract::models::TaskNotFoundException& ex) {
        return contract::models::ApiResponseFactory::NotFound(
            fmt::format("An error occurred while processing the request: {}", ex.what())
        );
    }

    return contract::models::ApiResponseFactory::Ok();
}

}  // namespace views::current_actions::v1::task::reactivate::post
