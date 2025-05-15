#pragma once

#include <userver/components/component_list.hpp>

#include "docs/yaml/api/api.hpp"

#include "../../../../../../clients/sometime_later/client.hpp"
#include "../../../../../../current_actions/contract/managers/tasks_manager.hpp"
#include "../../../../../../views/contract/base_handler.hpp"

namespace views::current_actions::v1::task::sometime::later::post {

class CurrentActionsV1TaskSometimeLaterPost final
    : public views::contract::BaseHandler< ::current_actions::handlers::TaskIdRequest> {
public:
    static constexpr std::string_view kName = "current-actions-v1-task-sometime-later-post";

    CurrentActionsV1TaskSometimeLaterPost(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context
    );

protected:
    views::contract::models::ApiResponse Handle(
        ::current_actions::handlers::TaskIdRequest&& request,
        userver::server::request::RequestContext&& context
    ) const override;

private:
    const ::current_actions::contract::managers::TasksManager& tasks_manager_;
    const ::clients::sometime_later::SometimeLaterClient& sometime_later_client_;
};

}  // namespace views::current_actions::v1::task::sometime::later::post