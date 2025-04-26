#include "../contract/managers/tasks_manager.hpp"

#include <userver/components/component_context.hpp>

#include "docs/yaml/api.hpp"

#include "../contract/models/exceptions.hpp"

namespace current_actions::contract::managers {

namespace {

using current_actions::models::TaskForCreate;
using current_actions::models::TaskId;
using current_actions::models::UserId;
using current_actions::models::Task;
using providers::tasks_provider::TasksProvider;

}  // namespace

TasksManager::TasksManager(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : userver::components::ComponentBase(config, component_context),
      tasks_provider_(component_context.FindComponent<TasksProvider>()) {}

TaskForCreate TasksManager::Transform(handlers::CreateTaskRequest&& create_task_request) const {
    std::vector<std::string> tag_names;
    if (create_task_request.tags.has_value()) {
        tag_names = std::move(create_task_request.tags.value());
    }

    return {
        UserId{create_task_request.user_id},
        std::move(create_task_request.description),
        std::move(create_task_request.project_id),
        ::current_actions::models::Transform(create_task_request.priority),
        std::move(tag_names)
    };
}

TaskId TasksManager::Transform(handlers::TaskIdRequest&& task_id_request) const {
    return TaskId{std::move(task_id_request.task_id)};
}

handlers::ListTasksResponse TasksManager::Transform(std::vector<Task>&& tasks, std::optional<std::string>&& cursor) const {
    using userver::utils::datetime::TimePointTz;

    std::vector<current_actions::handlers::Task> result_tasks;
    result_tasks.reserve(tasks.size());

    for (auto&& task : tasks) {
        std::vector<current_actions::handlers::Tag> tags;
        tags.reserve(task.tags.size());
        for (auto&& tag : task.tags) {
            tags.emplace_back(current_actions::handlers::Tag{
                tag.id,
                std::move(tag.name),
                TimePointTz{tag.created_at}
            });
        }
        
        result_tasks.emplace_back(current_actions::handlers::Task{
            std::move(task.id.GetUnderlying()),
            std::move(task.user_id.GetUnderlying()),
            std::move(task.description),
            ::current_actions::models::Transform(task.status).value(),
            std::move(task.project_id),
            ::current_actions::models::Transform(task.priority).value(),
            {std::move(tags)},
            TimePointTz(task.created_at),
            TimePointTz{task.updated_at},
            task.completed_at.has_value() ? std::make_optional(TimePointTz{task.completed_at.value()}) : std::nullopt
        });
    }

    return {
        std::move(result_tasks),
        std::move(cursor)
    };
}

void TasksManager::CreateTask(handlers::CreateTaskRequest&& task) const {
    tasks_provider_.InsertTask(Transform(std::move(task)));
}

void TasksManager::CompleteTask(handlers::TaskIdRequest&& task_id_request) const {
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

handlers::ListTasksResponse TasksManager::ListTasks(handlers::ListTasksRequest&& list_task_request) const {
    auto [cursor, tasks] = tasks_provider_.SelectTasks(UserId{list_task_request.user_id}, current_actions::models::DeserializeCursorFromString(list_task_request.cursor), ::current_actions::models::Transform(list_task_request.status));
    
    return Transform(std::move(tasks), SerializeCursorToString(cursor));
}

}  // namespace current_actions::contract::managerss