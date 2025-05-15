#pragma once

#include <userver/components/component_base.hpp>

#include "../contract/base_client.hpp"
#include "../contract/current_actions/client.hpp"

namespace clients::current_actions {

class CurrentActionsClient final : public userver::components::LoggableComponentBase,
                                   public contract::current_actions::Client,
                                   private contract::BaseClient {
public:
    static constexpr std::string_view kName = "current-actions-client";

    CurrentActionsClient(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    );

    void MoveTask(::current_actions::handlers::Task&& task) const override;

    static userver::yaml_config::Schema GetStaticConfigSchema();
};

}  // namespace clients::current_actions
