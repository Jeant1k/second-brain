#pragma once

#include "docs/yaml/clients/sometime_later/definitions.hpp"

namespace clients::contract::sometime_later {

class Client {
public:
    virtual ~Client() = default;

    virtual void MoveTask(::sometime_later::handlers::Task&& task) const = 0;
};

}  // namespace clients::contract::sometime_later