#include "tasks_provider.hpp"

#include <fmt/format.h>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <userver/components/component_context.hpp>
#include <userver/logging/log.hpp>
#include <userver/storages/postgres/component.hpp>

#include "sometime_later/sql_queries.hpp"

namespace sometime_later::providers::tasks_provider {

namespace {

using contract::models::Cursor;
using contract::models::Status;
using contract::models::Task;
using contract::models::TaskForCreate;
using contract::models::TaskForUpdate;
using contract::models::TaskId;
using contract::models::UserId;

constexpr std::int32_t kTasksLimit{10};

}  // namespace

TasksProvider::TasksProvider(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : userver::components::ComponentBase(config, component_context),
      pg_cluster_(component_context.FindComponent<userver::components::Postgres>("postgres-sometime-later").GetCluster()
      ) {}

void TasksProvider::InsertTask(TaskForCreate&& task) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        sql::kInsertTask,
        task.user_id.GetUnderlying(),
        task.name,
        task.description
    );

    auto task_id = boost::uuids::to_string((*result.cbegin())["id"].As<boost::uuids::uuid>());

    LOG_INFO() << fmt::format("Task with id = {} was inserted", std::move(task_id));
}

void TasksProvider::UpsertTask(Task&& task) const {
    LOG_INFO() << "created_at = " << task.created_at.GetUnderlying()
               << " updated_at = " << task.updated_at.GetUnderlying() << " completed_at = "
               << task.completed_at.value_or(userver::storages::postgres::TimePointTz{}).GetUnderlying();

    const auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        sql::kUpsertTask,
        task.id.GetUnderlying(),
        task.user_id.GetUnderlying(),
        task.name,
        task.description,
        task.status,
        task.created_at,
        task.updated_at,
        task.completed_at
    );

    const bool inserted = (*result.cbegin())["inserted"].As<bool>();

    LOG_INFO() << fmt::format(
        "Task with id = {} was {}", boost::uuids::to_string(task.id.GetUnderlying()), inserted ? "inserted" : "updated"
    );
}

std::optional<UserId> TasksProvider::SelectUserIdByTaskId(TaskId&& task_id) const {
    const auto user_id_opt = pg_cluster_
                                 ->Execute(
                                     userver::storages::postgres::ClusterHostType::kMaster,
                                     sql::kSelectUserIdByTaskId,
                                     task_id.GetUnderlying()
                                 )
                                 .AsOptionalSingleRow<std::int64_t>(userver::storages::postgres::kFieldTag);

    if (!user_id_opt.has_value()) {
        return std::nullopt;
    }

    return UserId{user_id_opt.value()};
}

TasksProvider::MarkTaskAsCompletedResult TasksProvider::MarkTaskAsCompleted(TaskId&& task_id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster, sql::kMarkTaskAsCompleted, task_id.GetUnderlying()
    );

    if (result.RowsAffected() == 0) {
        LOG_WARNING() << fmt::format(
            "Task with id = {} was not marked as completed", boost::uuids::to_string(task_id.GetUnderlying())
        );
        return MarkTaskAsCompletedResult::kTaskNotFound;
    }

    LOG_INFO(
    ) << fmt::format("Task with id = {} was marked as completed", boost::uuids::to_string(task_id.GetUnderlying()));
    return MarkTaskAsCompletedResult::kSuccess;
}

TasksProvider::MarkTaskAsPendingResult TasksProvider::MarkTaskAsPending(TaskId&& task_id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster, sql::kMarkTaskAsPending, task_id.GetUnderlying()
    );

    if (result.RowsAffected() == 0) {
        LOG_WARNING() << fmt::format(
            "Task with id = {} was not marked as pending", boost::uuids::to_string(task_id.GetUnderlying())
        );
        return MarkTaskAsPendingResult::kTaskNotFound;
    }

    LOG_INFO(
    ) << fmt::format("Task with id = {} was marked as active", boost::uuids::to_string(task_id.GetUnderlying()));
    return MarkTaskAsPendingResult::kSuccess;
}

TasksProvider::MarkTaskAsDeletedResult TasksProvider::MarkTaskAsDeleted(TaskId&& task_id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster, sql::kMarkTaskAsDeleted, task_id.GetUnderlying()
    );

    if (result.RowsAffected() == 0) {
        LOG_WARNING() << fmt::format(
            "Task with id = {} was not marked as deleted", boost::uuids::to_string(task_id.GetUnderlying())
        );
        return MarkTaskAsDeletedResult::kTaskNotFound;
    }

    LOG_INFO(
    ) << fmt::format("Task with id = {} was marked as deleted", boost::uuids::to_string(task_id.GetUnderlying()));
    return MarkTaskAsDeletedResult::kSuccess;
}

void TasksProvider::MarkTaskAsMovedToCurrentActions(contract::models::TaskId&& task_id) const {
    pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        sql::kMarkTaskAsMovedToCurrentActions,
        task_id.GetUnderlying()
    );
}

TasksProvider::SelectTaskByIdResult TasksProvider::SelectTaskById(contract::models::TaskId&& task_id) const {
    auto task =
        pg_cluster_
            ->Execute(
                userver::storages::postgres::ClusterHostType::kMaster, sql::kSelectTaskById, task_id.GetUnderlying()
            )
            .AsOptionalSingleRow<contract::models::Task>(userver::storages::postgres::kRowTag);

    if (!task.has_value()) {
        LOG_WARNING(
        ) << fmt::format("Task with id = {} was not found", boost::uuids::to_string(task_id.GetUnderlying()));
        return {SelectTaskByIdResult::SelectTaskByIdStatus::kTaskNotFound, std::nullopt};
    }

    return {SelectTaskByIdResult::SelectTaskByIdStatus::kSuccess, std::move(task)};
}

TasksProvider::UpdateTaskFieldsResult TasksProvider::UpdateTaskFields(TaskForUpdate&& task) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        sql::kUpdateNameOrDescription,
        task.task_id.GetUnderlying(),
        task.name,
        task.description
    );

    if (result.RowsAffected() == 0) {
        LOG_WARNING(
        ) << fmt::format("No task with id {} found for update", boost::uuids::to_string(task.task_id.GetUnderlying()));
        return UpdateTaskFieldsResult::kTaskNotFound;
    }

    LOG_INFO() << fmt::format("Task with id {} updated fields", boost::uuids::to_string(task.task_id.GetUnderlying()));
    return UpdateTaskFieldsResult::kSuccess;
}

TasksProvider::SelectTasksResult
TasksProvider::SelectTasks(UserId&& user_id, std::optional<Cursor>&& cursor, std::optional<Status>&& status) const {
    std::optional<userver::storages::postgres::TimePointTz> updated_at;
    std::optional<boost::uuids::uuid> task_id;
    if (cursor.has_value()) {
        updated_at = cursor.value().updated_at;
        task_id = std::move(cursor.value().id.GetUnderlying());
    }

    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        sql::kSelectTasks,
        user_id.GetUnderlying(),
        updated_at,
        task_id,
        status,
        kTasksLimit + 1
    );

    SelectTasksResult select_tasks_result;
    select_tasks_result.tasks = result.AsContainer<std::vector<Task>>(userver::storages::postgres::kRowTag);

    if (select_tasks_result.tasks.size() > kTasksLimit) {
        auto& last_task = select_tasks_result.tasks.back();
        select_tasks_result.cursor = {last_task.updated_at, last_task.id};

        select_tasks_result.tasks.resize(kTasksLimit);
    }

    LOG_INFO() << fmt::format("Selected {} tasks for user_id = {}", select_tasks_result.tasks.size(), user_id);

    return select_tasks_result;
}

}  // namespace sometime_later::providers::tasks_provider