#pragma once

#include "docs/yaml/definitions.hpp"

namespace clients::contract::current_actions {

class Client {
public:
    virtual ~Client() = default;

    virtual void MoveTask(::waiting::handlers::Task&& task) const = 0;
};

}  // namespace clients::contract::current_actions