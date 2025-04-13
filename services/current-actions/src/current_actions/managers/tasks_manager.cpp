#include "../contract/managers/tasks_manager.hpp"

#include <userver/components/component_context.hpp>

#include "docs/yaml/api.hpp"

namespace current_actions::contract::managers {

namespace {

using current_actions::handlers::CreateTaskRequest;
using current_actions::models::Task;
using current_actions::models::kMapPriority;
using current_actions::providers::tasks_provider::TasksProvider;

}  // namespace

TasksManager::TasksManager(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
) : userver::components::ComponentBase(config, component_context),
    tasks_provider_(component_context.FindComponent<TasksProvider>()) {}

Task TasksManager::Transform(CreateTaskRequest&& create_task_request) const {
    std::vector<std::string> tags;
    if (create_task_request.tags.has_value()) {
        tags.reserve(create_task_request.tags.value().size());
        for (auto&& name : create_task_request.tags.value()) {
            tags.emplace_back(std::move(name));
        }
    }

    return {
        create_task_request.user_id,
        std::move(create_task_request.description),
        std::move(create_task_request.project_id),
        kMapPriority.at(create_task_request.priority),
        std::move(tags)};
}

void TasksManager::CreateTask(CreateTaskRequest&& task) const {
    tasks_provider_.InsertTask(Transform(std::move(task)));
}

}  // namespace current_actions::contract::providers