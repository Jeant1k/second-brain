#pragma once

#include <userver/components/component_list.hpp>

#include "docs/yaml/api.hpp"

#include "../../../../../current_actions/contract/managers/tasks_manager.hpp"
#include "../../../../../views/contract/base_handler.hpp"

namespace views::current_actions::v1::task::remove::post {

class CurrentActionsV1TaskRemovePost final
    : public views::contract::BaseHandler< ::current_actions::handlers::TaskIdRequest> {
public:
    static constexpr std::string_view kName = "current-actions-v1-task-remove-post";

    CurrentActionsV1TaskRemovePost(
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
};

}  // namespace views::current_actions::v1::task::remove::post
