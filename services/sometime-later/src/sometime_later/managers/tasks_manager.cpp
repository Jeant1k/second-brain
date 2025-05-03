#include "../contract/managers/tasks_manager.hpp"

#include <userver/components/component_context.hpp>

#include "docs/yaml/api.hpp"
#include "docs/yaml/internal.hpp"

#include "../contract/models/exceptions.hpp"

namespace sometime_later::contract::managers {

namespace {

using models::Task;
using models::TaskForCreate;
using models::TaskForUpdate;
using models::TaskId;
using models::UserId;
using providers::tasks_provider::TasksProvider;

}  // namespace

TasksManager::TasksManager(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : userver::components::ComponentBase(config, component_context),
      tasks_provider_(component_context.FindComponent<TasksProvider>()) {}

void TasksManager::CreateTask(handlers::CreateTaskRequest&& task) const {
    tasks_provider_.InsertTask(Transform(std::move(task)));
}

void TasksManager::MoveTask(handlers::MoveTaskRequest&& move_task_request) const {
    const auto user_id_opt = tasks_provider_.SelectUserIdByTaskId(TaskId{move_task_request.task.id});
    if (user_id_opt.has_value() && user_id_opt.value().GetUnderlying() != move_task_request.task.user_id) {
        throw models::WrongUserIdException{
            fmt::format("Task with id = {} belongs to another user", boost::uuids::to_string(move_task_request.task.id))
        };
    }

    tasks_provider_.UpsertTask(Transform(std::move(move_task_request)));
}

void TasksManager::UpdateTask(handlers::UpdateTaskRequest&& update_task_request) const {
    if (!update_task_request.name.has_value() && !update_task_request.description.has_value()) {
        throw models::NoFieldsProvidedException{"No task fields provided for update"};
    }

    const auto result = tasks_provider_.UpdateTaskFields(Transform(std::move(update_task_request)));

    if (result == TasksProvider::UpdateTaskFieldsResult::kTaskNotFound) {
        throw models::TaskNotFoundException{"Task to update fields was not found"};
    }
}

void TasksManager::CompleteTask(handlers::TaskIdRequest&& task_id_request) const {
    const auto result = tasks_provider_.MarkTaskAsCompleted(Transform(std::move(task_id_request)));

    if (result == TasksProvider::MarkTaskAsCompletedResult::kTaskNotFound) {
        throw models::TaskNotFoundException{"Task to mark as completed was not found"};
    }
}

void TasksManager::PendTask(handlers::TaskIdRequest&& task_id_request) const {
    const auto result = tasks_provider_.MarkTaskAsPending(Transform(std::move(task_id_request)));

    if (result == TasksProvider::MarkTaskAsPendingResult::kTaskNotFound) {
        throw models::TaskNotFoundException{"Task to mark as pending was not found"};
    }
}

void TasksManager::DeleteTask(handlers::TaskIdRequest&& task_id_request) const {
    const auto result = tasks_provider_.MarkTaskAsDeleted(Transform(std::move(task_id_request)));

    if (result == TasksProvider::MarkTaskAsDeletedResult::kTaskNotFound) {
        throw models::TaskNotFoundException{"Task to mark as deleted was not found"};
    }
}

handlers::ListTasksResponse TasksManager::ListTasks(handlers::ListTasksRequest&& list_task_request) const {
    auto [cursor, tasks] = tasks_provider_.SelectTasks(
        UserId{list_task_request.user_id},
        models::DeserializeCursorFromString(list_task_request.cursor),
        models::Transform(list_task_request.status)
    );

    return Transform(std::move(tasks), SerializeCursorToString(cursor));
}

models::Task TasksManager::MoveToCurrentActionsTask(handlers::TaskIdRequest&& task_id_request) const {
    const auto result = tasks_provider_.MarkTaskAsMovedToCurrentActions(Transform(std::move(task_id_request)));
    if (result.status == TasksProvider::MarkTaskAsMovedToCurrentActionsResult::MarkTaskAsMovedToCurrentActionsStatus::
                             kTaskNotFound ||
        !result.task.has_value()) {
        throw models::TaskNotFoundException{"Task to move to current actions was not found"};
    }

    return result.task.value();
}

TaskForCreate TasksManager::Transform(handlers::CreateTaskRequest&& create_task_request) const {
    return {
        UserId{create_task_request.user_id},
        std::move(create_task_request.name),
        std::move(create_task_request.description),
    };
}

TaskId TasksManager::Transform(handlers::TaskIdRequest&& task_id_request) const {
    return TaskId{std::move(task_id_request.task_id)};
}

handlers::ListTasksResponse TasksManager::Transform(std::vector<Task>&& tasks, std::optional<std::string>&& cursor)
    const {
    using userver::utils::datetime::TimePointTz;

    std::vector<sometime_later::handlers::Task> result_tasks;
    result_tasks.reserve(tasks.size());

    for (auto&& task : tasks) {
        result_tasks.emplace_back(sometime_later::handlers::Task{
            std::move(task.id.GetUnderlying()),
            std::move(task.user_id.GetUnderlying()),
            std::move(task.name),
            std::move(task.description),
            models::Transform(task.status).value(),
            TimePointTz(task.created_at),
            TimePointTz{task.updated_at},
            task.completed_at.has_value() ? std::make_optional(TimePointTz{task.completed_at.value()}) : std::nullopt
        });
    }

    return {std::move(result_tasks), std::move(cursor)};
}

TaskForUpdate TasksManager::Transform(handlers::UpdateTaskRequest&& update_task_request) const {
    return {
        TaskId{update_task_request.task_id},
        std::move(update_task_request.name),
        std::move(update_task_request.description)
    };
}

Task TasksManager::Transform(handlers::MoveTaskRequest&& move_task_request) const {
    using userver::storages::postgres::TimePointTz;

    LOG_INFO() << "created_at = "
               << move_task_request.task.created_at.value_or(userver::utils::datetime::TimePointTz{}).GetTimePoint()
               << " updated_at = "
               << move_task_request.task.updated_at.value_or(userver::utils::datetime::TimePointTz{}).GetTimePoint()
               << " completed_at = "
               << move_task_request.task.completed_at.value_or(userver::utils::datetime::TimePointTz{}).GetTimePoint();

    return {
        TaskId{move_task_request.task.id},
        UserId{move_task_request.task.user_id},
        std::move(move_task_request.task.name),
        std::move(move_task_request.task.description),
        models::Transform(move_task_request.task.status).value(),
        move_task_request.task.created_at.has_value() ? TimePointTz{move_task_request.task.created_at.value()}
                                                      : TimePointTz{userver::utils::datetime::Now()},
        move_task_request.task.updated_at.has_value() ? TimePointTz{move_task_request.task.updated_at.value()}
                                                      : TimePointTz{userver::utils::datetime::Now()},
        move_task_request.task.completed_at.has_value() ? TimePointTz{move_task_request.task.completed_at.value()}
                                                        : TimePointTz{userver::utils::datetime::Now()},
    };
}

}  // namespace sometime_later::contract::managers