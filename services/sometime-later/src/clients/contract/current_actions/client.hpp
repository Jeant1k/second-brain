#pragma once

#include "docs/yaml/clients/current_actions/definitions.hpp"

namespace clients::contract::current_actions {

class Client {
public:
    virtual ~Client() = default;

    virtual void MoveTask(::current_actions::handlers::Task&& task) const = 0;
};

}  // namespace clients::contract::current_actions