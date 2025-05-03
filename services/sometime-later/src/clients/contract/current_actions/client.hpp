#pragma once

#include <userver/components/loggable_component_base.hpp>

#include "docs/yaml/definitions.hpp"  // для Task
#include "docs/yaml/internal.hpp"     // Для MoveTaskRequest

namespace clients::contract::current_actions {

// Интерфейс клиента (для удобства мокирования и DI)
class Client {
public:
    virtual ~Client() = default;

    // Метод для перемещения задачи
    virtual void MoveTask(const ::sometime_later::handlers::Task& task) const = 0;
    // В будущем здесь могут быть другие методы API current-actions
};

}  // namespace clients::contract::current_actions
