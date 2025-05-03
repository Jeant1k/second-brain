#pragma once

#include <string>

#include <userver/clients/http/component.hpp>     // http::ClientComponent
#include <userver/components/component_base.hpp>  // ComponentBase

#include "../contract/base_client.hpp"
#include "../contract/current_actions/client.hpp"  // Подключаем интерфейс

namespace clients::current_actions {

class CurrentActionsClient final : public userver::components::LoggableComponentBase,
                                   public contract::current_actions::Client  // Реализуем интерфейс
{
public:
    // Имя компонента
    static constexpr std::string_view kName = "current-actions-client";

    CurrentActionsClient(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    );

    // Реализация метода интерфейса
    void MoveTask(const ::sometime_later::handlers::Task& task) const override;

    // Реализация виртуального метода из BaseClient
    std::string GetClientName() const override { return std::string(kName); }

private:
    userver::clients::http::Client& http_client_;
    contract::BaseClient base_client_;  // Используем BaseClient для выполнения запросов
};

}  // namespace clients::current_actions
