#pragma once

#include <userver/components/component_base.hpp>
#include <userver/storages/postgres/cluster.hpp>

#include "../../models/tasks_provider_structures.hpp"

namespace current_actions::providers::tasks_provider {

class TasksProvider final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "tasks-provider";

    TasksProvider(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context
    );

    void InsertTask(models::TaskForCreate&& task) const;

    void UpsertTask(models::Task&& task) const;

    std::optional<models::UserId> SelectUserIdByTaskId(models::TaskId&& task_id) const;

    struct SelectTaskByIdResult {
        enum class SelectTaskByIdStatus : char { kSuccess, kTaskNotFound };

        SelectTaskByIdStatus status;
        std::optional<models::Task> task;
    };
    SelectTaskByIdResult SelectTaskById(models::TaskId&& task_id) const;

    enum class MarkTaskAsCompletedResult : char { kSuccess, kTaskNotFound };
    MarkTaskAsCompletedResult MarkTaskAsCompleted(models::TaskId&& task_id) const;

    enum class MarkTaskAsActiveResult : char { kSuccess, kTaskNotFound };
    MarkTaskAsActiveResult MarkTaskAsActive(models::TaskId&& task_id) const;

    enum class MarkTaskAsDeletedResult : char { kSuccess, kTaskNotFound };
    MarkTaskAsDeletedResult MarkTaskAsDeleted(models::TaskId&& task_id) const;

    void MarkTaskAsMovedToSometimeLater(models::TaskId&& task_id) const;

    enum class UpdateTaskFieldsResult : char { kSuccess, kTaskNotFound };
    UpdateTaskFieldsResult UpdateTaskFields(models::TaskForUpdate&& task) const;

    struct SelectTasksResult {
        std::optional<models::Cursor> cursor;
        std::vector<models::Task> tasks;
    };
    SelectTasksResult SelectTasks(
        models::UserId&& user_id,
        std::optional<models::Cursor>&& cursor = std::nullopt,
        std::optional<models::Status>&& status = std::nullopt
    ) const;

private:
    const userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace current_actions::providers::tasks_provider
