#include "view.hpp"

#include <userver/components/component.hpp>

#include "docs/yaml/api.hpp"

#include "../../../../../../clients/contract/models/exceptions.hpp"
#include "../../../../../../current_actions/contract/models/exceptions.hpp"
// TODO: Перенести в "../../../../../../current_actions/contract/models/tasks_provider_structures.hpp"
#include "../../../../../../current_actions/models/tasks_provider_structures.hpp"

namespace views::current_actions::v1::task::sometime::later::post {

namespace {

::current_actions::handlers::Task Transform(::current_actions::models::Task&& task) {
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

CurrentActionsV1TaskSometimeLaterPost::CurrentActionsV1TaskSometimeLaterPost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::current_actions::handlers::TaskIdRequest>(config, component_context),
      tasks_manager_(component_context.FindComponent<::current_actions::contract::managers::TasksManager>()),
      sometime_later_client_(component_context.FindComponent<::clients::sometime_later::SometimeLaterClient>()) {}

views::contract::models::ApiResponse CurrentActionsV1TaskSometimeLaterPost::
    Handle(::current_actions::handlers::TaskIdRequest&& request, userver::server::request::RequestContext&&) const {
    ::current_actions::models::Task task;

    try {
        task = tasks_manager_.GetTask(std::move(request));
    } catch (const ::current_actions::contract::models::TaskNotFoundException& ex) {
        return contract::models::ApiResponseFactory::NotFound(
            fmt::format("An error occurred while processing the request: {}", ex.what())
        );
    }

    if (task.status == ::current_actions::models::Status::kMovedToSometimeLater) {
        return contract::models::ApiResponseFactory::BadRequest(fmt::format(
            "Task with id = {} was already moved to sometime-later", boost::uuids::to_string(task.id.GetUnderlying())
        ));
    }

    auto task_id = task.id;

    try {
        sometime_later_client_.MoveTask(Transform(std::move(task)));
    } catch (const ::clients::contract::models::BadRequestError& ex) {
        return contract::models::ApiResponseFactory::BadRequest(
            fmt::format("Client error while moving task: {}", ex.what())
        );
    }

    tasks_manager_.SometimeLaterTask(std::move(task_id));

    return contract::models::ApiResponseFactory::Ok();
}

}  // namespace views::current_actions::v1::task::sometime::later::post