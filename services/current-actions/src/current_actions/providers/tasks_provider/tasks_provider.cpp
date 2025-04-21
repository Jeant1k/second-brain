#include "tasks_provider.hpp"

#include <fmt/format.h>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <userver/components/component_context.hpp>
#include <userver/logging/log.hpp>
#include <userver/storages/postgres/component.hpp>

#include "current_actions/sql_queries.hpp"

namespace current_actions::providers::tasks_provider {

namespace {

using models::TaskInfo;
using models::TaskId;
using models::Cursor;
using models::FullTaskInfo;

constexpr std::size_t kTasksLimit{20};

}  // namespace

TasksProvider::TasksProvider(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : userver::components::ComponentBase(config, component_context),
      pg_cluster_(
          component_context.FindComponent<userver::components::Postgres>("postgres-current-actions").GetCluster()
      ) {}

void TasksProvider::InsertTask(TaskInfo&& task) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        sql::kInsertTask,
        task.user_id,
        std::move(task.description),
        std::move(task.project_id),
        task.priority,
        std::move(task.tags)
    );

    auto task_id = boost::uuids::to_string((*result.cbegin())["id"].As<boost::uuids::uuid>());

    LOG_INFO() << fmt::format("Task with id = {} was inserted", std::move(task_id));
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

TasksProvider::MarkTaskAsActiveResult TasksProvider::MarkTaskAsActive(TaskId&& task_id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster, sql::kMarkTaskAsActive, task_id.GetUnderlying()
    );

    if (result.RowsAffected() == 0) {
        LOG_WARNING() << fmt::format(
            "Task with id = {} was not marked as active", boost::uuids::to_string(task_id.GetUnderlying())
        );
        return MarkTaskAsActiveResult::kTaskNotFound;
    }

    LOG_INFO(
    ) << fmt::format("Task with id = {} was marked as active", boost::uuids::to_string(task_id.GetUnderlying()));
    return MarkTaskAsActiveResult::kSuccess;
}

TasksProvider::SelectTasksResult TasksProvider::SelectTasks(models::UserId&& user_id, std::optional<models::Cursor>&& cursor, std::optional<models::Status>&& status) const {
    boost::uuids::uuid cursor_id{};
    
    if (cursor) {
        cursor_id = cursor->id.GetUnderlying();
    }
    
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        sql::kSelectTasks,
        user_id.GetUnderlying(),
        cursor ? cursor_id : nullptr,
        status,
        kTasksLimit + 1
    );
    
    auto tasks = result.AsContainer<std::vector<FullTaskInfo>>(userver::storages::postgres::kRowTag);
    
    SelectTasksResult select_result;
    
    if (tasks.size() > kTasksLimit) {
        auto& last_task = tasks[kTasksLimit - 1];
        
        models::Cursor new_cursor{
            last_task.updated_at,
            last_task.id
        };
        select_result.cursor = std::move(new_cursor);
        
        tasks.resize(kTasksLimit);
    }
    
    select_result.tasks = std::move(tasks);
    
    return select_result;



    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        sql::kSelectTasks,
        cursor.updated_at,
        cursor.id.GetUnderlying(),
        kTasksLimit
    );

    auto tasks = result.AsContainer<std::vector<FullTaskInfo>>(userver::storages::postgres::kRowTag);
    LOG_INFO() << fmt::format("Selected {} tasks", tasks.size());

    const auto& last_row = *std::prev(result.end());
    auto min_updated_at = last_row["min_updated_at"].As<userver::storages::postgres::TimePointTz>();
    auto min_id = last_row["min_id"].As<boost::uuids::uuid>();
    auto next_cursor = Cursor{std::move(min_updated_at), std::move(TaskId{min_id})};
    
    return {std::move(next_cursor), std::move(tasks)};
}

}  // namespace current_actions::providers::tasks_provider