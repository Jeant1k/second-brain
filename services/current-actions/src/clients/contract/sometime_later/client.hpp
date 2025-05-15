#pragma once

#include "docs/yaml/definitions.hpp"

namespace clients::contract::sometime_later {

class Client {
public:
    virtual ~Client() = default;

    virtual void MoveTask(::current_actions::handlers::Task&& task) const = 0;
};

}  // namespace clients::contract::sometime_later