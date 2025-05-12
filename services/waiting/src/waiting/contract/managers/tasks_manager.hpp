#pragma once

#include <userver/components/component_base.hpp>

#include "../../providers/tasks_provider/tasks_provider.hpp"
#include "../models/tasks_provider_structures.hpp"

#include "docs/yaml/api_fwd.hpp"
#include "docs/yaml/internal_fwd.hpp"

namespace waiting::contract::managers {

class TasksManager final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "tasks-manager";

    TasksManager(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context
    );

    models::Task GetTask(handlers::TaskIdRequest&& task_id_request) const;

    void CreateTask(handlers::CreateTaskRequest&& create_task_request) const;

    void MoveTask(handlers::MoveTaskRequest&& move_task_request) const;

    void UpdateTask(handlers::UpdateTaskRequest&& update_task_request) const;

    void CompleteTask(handlers::TaskIdRequest&& task_id_request) const;

    void ReactivateTask(handlers::TaskIdRequest&& task_id_request) const;

    void DeleteTask(handlers::TaskIdRequest&& task_id_request) const;

    void CurrentActionsTask(models::TaskId&& task_id) const;

    handlers::ListTasksResponse ListTasks(handlers::ListTasksRequest&& list_task_request) const;

private:
    models::TaskForCreate Transform(handlers::CreateTaskRequest&& create_task_request) const;
    models::TaskId Transform(handlers::TaskIdRequest&& task_id_request) const;
    handlers::ListTasksResponse Transform(std::vector<models::Task>&& tasks, std::optional<std::string>&& cursor) const;
    models::TaskForUpdate Transform(handlers::UpdateTaskRequest&& update_task_request) const;
    models::Task Transform(handlers::MoveTaskRequest&& move_task_request) const;

private:
    const providers::tasks_provider::TasksProvider& tasks_provider_;
};

}  // namespace waiting::contract::managers
