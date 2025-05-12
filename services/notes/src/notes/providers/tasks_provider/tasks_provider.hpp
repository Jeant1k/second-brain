#pragma once

#include <userver/components/component_base.hpp>
#include <userver/storages/postgres/cluster.hpp>

#include "../../contract/models/tasks_provider_structures.hpp"

namespace notes::providers::tasks_provider {

class TasksProvider final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "tasks-provider";

    TasksProvider(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context
    );

    void InsertTask(contract::models::TaskForCreate&& task) const;

    void UpsertTask(contract::models::Task&& task) const;

    std::optional<contract::models::UserId> SelectUserIdByTaskId(contract::models::TaskId&& task_id) const;

    enum class MarkTaskAsCompletedResult : char { kSuccess, kTaskNotFound };
    MarkTaskAsCompletedResult MarkTaskAsCompleted(contract::models::TaskId&& task_id) const;

    enum class MarkTaskAsActiveResult : char { kSuccess, kTaskNotFound };
    MarkTaskAsActiveResult MarkTaskAsActive(contract::models::TaskId&& task_id) const;

    enum class MarkTaskAsDeletedResult : char { kSuccess, kTaskNotFound };
    MarkTaskAsDeletedResult MarkTaskAsDeleted(contract::models::TaskId&& task_id) const;

    void MarkTaskAsMovedToCurrentActions(contract::models::TaskId&& task_id) const;

    struct SelectTaskByIdResult {
        enum class SelectTaskByIdStatus : char { kSuccess, kTaskNotFound };

        SelectTaskByIdStatus status;
        std::optional<contract::models::Task> task;
    };
    SelectTaskByIdResult SelectTaskById(contract::models::TaskId&& task_id) const;

    enum class UpdateTaskFieldsResult : char { kSuccess, kTaskNotFound };
    UpdateTaskFieldsResult UpdateTaskFields(contract::models::TaskForUpdate&& task) const;

    struct SelectTasksResult {
        std::optional<contract::models::Cursor> cursor;
        std::vector<contract::models::Task> tasks;
    };
    SelectTasksResult SelectTasks(
        contract::models::UserId&& user_id,
        std::optional<contract::models::Cursor>&& cursor = std::nullopt,
        std::optional<contract::models::Status>&& status = std::nullopt
    ) const;

private:
    const userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace notes::providers::tasks_provider
