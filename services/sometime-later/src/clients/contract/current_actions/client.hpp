#pragma once

#include "docs/yaml/definitions.hpp"

namespace clients::contract::current_actions {

class Client {
public:
    virtual ~Client() = default;

    virtual void MoveTask(::sometime_later::handlers::Task&& task) const = 0;
};

}  // namespace clients::contract::current_actions