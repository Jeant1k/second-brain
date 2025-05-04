#pragma once

#include <userver/components/component_list.hpp>

#include "docs/yaml/api.hpp"

#include "../../../../../../sometime_later/contract/managers/tasks_manager.hpp"
#include "../../../../../../views/contract/base_handler.hpp"
#include "../../../../../../clients/contract/current_actions/client.hpp"

namespace views::sometime_later::v1::task::current::actions::post {

class SometimeLaterV1TaskCurrentActionsPost final
    : public views::contract::BaseHandler< ::sometime_later::handlers::TaskIdRequest> {
public:
    static constexpr std::string_view kName = "sometime-later-v1-task-current-actions-post";

    SometimeLaterV1TaskCurrentActionsPost(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context
    );

protected:
    views::contract::models::ApiResponse Handle(
        ::sometime_later::handlers::TaskIdRequest&& request,
        userver::server::request::RequestContext&& context
    ) const override;

private:
    const ::sometime_later::contract::managers::TasksManager& tasks_manager_;
    const ::clients::contract::current_actions::Client& current_actions_client_;
};

}  // namespace views::sometime_later::v1::task::current::actions::post
