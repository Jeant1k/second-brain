#pragma once

#include <userver/components/component_list.hpp>

#include "docs/yaml/api.hpp"

#include "../../../../../../clients/current_actions/client.hpp"
#include "../../../../../../views/contract/base_handler.hpp"
#include "../../../../../../waiting/contract/managers/tasks_manager.hpp"

namespace views::waiting::v1::task::current::actions::post {

class WaitingV1TaskCurrentActionsPost final : public views::contract::BaseHandler< ::waiting::handlers::TaskIdRequest> {
public:
    static constexpr std::string_view kName = "waiting-v1-task-current-actions-post";

    WaitingV1TaskCurrentActionsPost(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context
    );

protected:
    views::contract::models::ApiResponse Handle(
        ::waiting::handlers::TaskIdRequest&& request,
        userver::server::request::RequestContext&& context
    ) const override;

private:
    const ::waiting::contract::managers::TasksManager& tasks_manager_;
    const ::clients::current_actions::CurrentActionsClient& current_actions_client_;
};

}  // namespace views::waiting::v1::task::current::actions::post
