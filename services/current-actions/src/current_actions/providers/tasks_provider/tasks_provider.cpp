#include "tasks_provider.hpp"

#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/logging/log.hpp>
#include <fmt/format.h>

#include "current_actions/sql_queries.hpp"

namespace current_actions::providers::tasks_provider {

namespace {

using current_actions::models::Task;

}  // namespace

TasksProvider::TasksProvider(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
) : userver::components::ComponentBase(config, component_context),
    pg_cluster_(component_context.FindComponent<userver::components::Postgres>("postgres-current-actions").GetCluster())
    {}

void TasksProvider::InsertTask(Task&& task) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        sql::kInsertTask,
        task.user_id,
        std::move(task.description),
        std::move(task.project_id),
        task.priority,
        std::move(task.tags)
    );

    LOG_INFO() << fmt::format("Task with id = {} has been inserted", result.AsSingleRow<std::int64_t>());
}

}  // namespace current_actions::providers::tasks_provider