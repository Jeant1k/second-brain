#include "../contract/managers/tasks_manager.hpp"

#include <userver/components/component_context.hpp>

#include "../contract/models/exceptions.hpp"
#include "docs/yaml/api.hpp"

namespace current_actions::contract::managers {

namespace {

using current_actions::models::kMapPriority;
using current_actions::models::TaskInfo;
using current_actions::models::TaskId;
using handlers::CreateTaskRequest;
using handlers::TaskIdRequest;
using handlers::ListTasksRequest;
using handlers::ListTasksResponse;
using providers::tasks_provider::TasksProvider;

}  // namespace

TasksManager::TasksManager(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : userver::components::ComponentBase(config, component_context),
      tasks_provider_(component_context.FindComponent<TasksProvider>()) {}

TaskInfo TasksManager::Transform(CreateTaskRequest&& create_task_request) const {
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
        std::move(tags)
    };
}

TaskId TasksManager::Transform(TaskIdRequest&& task_id_request) const {
    return TaskId{std::move(task_id_request.task_id)};
}

void TasksManager::CreateTask(CreateTaskRequest&& task) const {
    tasks_provider_.InsertTask(Transform(std::move(task)));
}

void TasksManager::CompleteTask(TaskIdRequest&& task_id_request) const {
    const auto result = tasks_provider_.MarkTaskAsCompleted(Transform(std::move(task_id_request)));

    if (result == TasksProvider::MarkTaskAsCompletedResult::kTaskNotFound) {
        throw models::TaskNotFoundException{"Task to mark as completed was not found"};
    }
}

void TasksManager::ReactivateTask(handlers::TaskIdRequest&& task_id_request) const {
    const auto result = tasks_provider_.MarkTaskAsActive(Transform(std::move(task_id_request)));

    if (result == TasksProvider::MarkTaskAsActiveResult::kTaskNotFound) {
        throw models::TaskNotFoundException{"Task to mark as active was not found"};
    }
}

ListTasksResponse TasksManager::ListTasks(ListTasksRequest&& list_task_request) const {
    auto result = tasks_provider_.SelectTasks()
}

}  // namespace current_actions::contract::managers