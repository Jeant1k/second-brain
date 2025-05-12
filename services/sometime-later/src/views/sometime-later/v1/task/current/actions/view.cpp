#include "view.hpp"

#include <userver/components/component.hpp>

#include "docs/yaml/api.hpp"

#include "../../../../../../clients/contract/models/exceptions.hpp"
#include "../../../../../../sometime_later/contract/models/exceptions.hpp"
#include "../../../../../../sometime_later/contract/models/tasks_provider_structures.hpp"

namespace views::sometime_later::v1::task::current::actions::post {

namespace {

::sometime_later::handlers::Task Transform(::sometime_later::contract::models::Task&& task) {
    using userver::utils::datetime::TimePointTz;

    return {
        task.id.GetUnderlying(),
        task.user_id.GetUnderlying(),
        std::move(task.name),
        std::move(task.description),
        Transform(task.status).value(),
        TimePointTz{task.created_at},
        TimePointTz{task.updated_at},
        task.completed_at.has_value() ? std::make_optional(TimePointTz{task.completed_at.value()}) : std::nullopt
    };
}

}  // namespace

SometimeLaterV1TaskCurrentActionsPost::SometimeLaterV1TaskCurrentActionsPost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::sometime_later::handlers::TaskIdRequest>(config, component_context),
      tasks_manager_(component_context.FindComponent<::sometime_later::contract::managers::TasksManager>()),
      current_actions_client_(component_context.FindComponent<::clients::current_actions::CurrentActionsClient>()) {}

views::contract::models::ApiResponse SometimeLaterV1TaskCurrentActionsPost::
    Handle(::sometime_later::handlers::TaskIdRequest&& request, userver::server::request::RequestContext&&) const {
    ::sometime_later::contract::models::Task task;

    try {
        task = tasks_manager_.GetTask(std::move(request));
    } catch (const ::sometime_later::contract::models::TaskNotFoundException& ex) {
        return contract::models::ApiResponseFactory::NotFound(
            fmt::format("An error occurred while processing the request: {}", ex.what())
        );
    }

    if (task.status == ::sometime_later::contract::models::Status::kMovedToCurrentActions) {
        return contract::models::ApiResponseFactory::BadRequest(fmt::format(
            "Task with id = {} was already moved to current-actions", boost::uuids::to_string(task.id.GetUnderlying())
        ));
    }

    auto task_id = task.id;

    try {
        current_actions_client_.MoveTask(Transform(std::move(task)));
    } catch (const ::clients::contract::models::BadRequestError& ex) {
        return contract::models::ApiResponseFactory::BadRequest(
            fmt::format("Client error while moving task: {}", ex.what())
        );
    }

    tasks_manager_.CurrentActionsTask(std::move(task_id));

    return contract::models::ApiResponseFactory::Ok();
}

}  // namespace views::sometime_later::v1::task::current::actions::post
