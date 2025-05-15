#pragma once

#include <userver/components/component_base.hpp>

#include "../contract/base_client.hpp"
#include "../contract/sometime_later/client.hpp"

namespace clients::sometime_later {

// TODO: Использовать во всех клиентах кодогенерацию клиентских структур, а не аналогичных структур текущего сервиса

class SometimeLaterClient final : public userver::components::LoggableComponentBase,
                                  public contract::sometime_later::Client,
                                  private contract::BaseClient {
public:
    static constexpr std::string_view kName = "sometime-later-client";

    SometimeLaterClient(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    );

    void MoveTask(::current_actions::handlers::Task&& task) const override;

    static userver::yaml_config::Schema GetStaticConfigSchema();
};

}  // namespace clients::sometime_later