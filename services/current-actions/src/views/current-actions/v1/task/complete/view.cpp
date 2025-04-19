#include "view.hpp"

#include <userver/components/component.hpp>

#include "docs/yaml/api.hpp"

#include "../../../../../current_actions/contract/models/exceptions.hpp"

namespace views::current_actions::v1::task::complete::post {

namespace {

using ::current_actions::contract::managers::TasksManager;
using ::current_actions::handlers::TaskIdRequest;
using contract::models::ApiResponseFactory;
using views::contract::models::ApiResponse;
using ::current_actions::contract::models::TaskNotFoundException;

}  // namespace

CurrentActionsV1TaskCompletePost::CurrentActionsV1TaskCompletePost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
) : views::contract::BaseHandler<TaskIdRequest>(config, component_context),
    tasks_manager_(component_context.FindComponent<TasksManager>()) {}

ApiResponse CurrentActionsV1TaskCompletePost::Handle(
    TaskIdRequest&& request,
    userver::server::request::RequestContext&&
) const {
    try {
        tasks_manager_.CompleteTask(std::move(request));
    } catch (const TaskNotFoundException& ex) {
        return ApiResponseFactory::NotFound(
            fmt::format("An error occurred while processing the request: {}", ex.what()));
    }

    return ApiResponseFactory::Ok();
}

}  // namespace views::current_actions::v1::task::complete::post
