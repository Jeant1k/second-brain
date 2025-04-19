#pragma once

#include <userver/components/component_base.hpp>
#include <userver/storages/postgres/cluster.hpp>

#include "../../models/task.hpp"

namespace current_actions::providers::tasks_provider {

class TasksProvider final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "tasks-provider";

    TasksProvider(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context
    );

    void InsertTask(models::Task&& task) const;

    enum class MarkTaskAsCompletedResult : char { kSuccess, kTaskNotFound };
    MarkTaskAsCompletedResult MarkTaskAsCompleted(models::TaskId&& task_id) const;

    enum class MarkTaskAsActiveResult : char { kSuccess, kTaskNotFound };
    MarkTaskAsActiveResult MarkTaskAsActive(models::TaskId&& task_id) const;

private:
    const userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace current_actions::providers::tasks_provider
