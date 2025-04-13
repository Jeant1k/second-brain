#pragma once

#include <userver/components/component_base.hpp>

#include "docs/yaml/api_fwd.hpp"

#include "../../models/task.hpp"
#include "../../providers/tasks_provider/tasks_provider.hpp"

namespace current_actions::contract::managers {

class TasksManager final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "tasks-manager";

    TasksManager(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context);

    void CreateTask(current_actions::handlers::CreateTaskRequest&& create_task_request) const;

private:
    models::Task Transform(current_actions::handlers::CreateTaskRequest&& create_task_request) const;

private:
    const providers::tasks_provider::TasksProvider& tasks_provider_;
};
       
}  // namespace current_actions::contract::providers
