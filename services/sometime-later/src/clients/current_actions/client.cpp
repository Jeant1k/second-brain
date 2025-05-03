// src/clients/current_actions/client.cpp
#include "client.hpp"

#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/yaml_config/merge_schemas.hpp>  // Для схемы конфига

namespace clients {

CurrentActionsClient::CurrentActionsClient(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
)
    : userver::components::LoggableComponentBase(config, context),
      // Получаем компонент HTTP-клиента
      http_client_(context.FindComponent<userver::components::HttpClient>().GetHttpClient()),
      // Инициализируем BaseClient с нужным URL из конфига
      base_client_(http_client_, config["base-url"].As<std::string>()) {
    // Можно добавить проверку базового URL или другие инициализации
}

void CurrentActionsClient::MoveTask(const ::sometime_later::handlers::Task& task) const {
    // Формируем тело запроса используя кодогенеренную структуру
    ::sometime_later::handlers::internal::MoveTaskRequest request_body;
    request_body.task = task;  // Копируем или перемещаем данные задачи

    // Вызываем PerformRequest из BaseClient
    // Указываем void как ожидаемый тип ответа (для 200 OK без тела)
    // Указываем MoveTaskRequest как тип тела запроса
    base_client_.PerformRequest<void, ::sometime_later::handlers::internal::MoveTaskRequest>(
        userver::clients::http::HttpMethod::kPost,  // Метод
        "/internal/current-actions/v1/task/move",   // Путь
        request_body                                // Тело запроса
        // Заголовки и таймаут можно передать при необходимости
    );
}

// Статическая схема конфигурации компонента
userver::yaml_config::Schema CurrentActionsClient::GetStaticConfigSchema() {
    return userver::yaml_config::MergeSchemas<userver::components::LoggableComponentBase>(R"(
type: object
description: Client for the internal current-actions service.
additionalProperties: false
properties:
    base-url:
        type: string
        description: Base URL of the current-actions service (e.g., http://current-actions-service)
)");
}

}  // namespace clients

// Регистрация компонента (можно вынести в отдельный файл или в main.cpp)
#include <userver/components/minimal_component_list.hpp>
#include <userver/utils/daemon_run.hpp>

// ... main.cpp или другое место инициализации
// userver::components::ComponentList component_list = userver::components::MinimalComponentList();
// component_list.Append<clients::CurrentActionsClient>();
// component_list.Append<userver::components::HttpClient>(); // Не забыть добавить HTTP клиент
// Добавить в component_list там, где она у вас определяется.
