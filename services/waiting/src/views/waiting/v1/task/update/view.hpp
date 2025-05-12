#pragma once

#include <userver/components/component_list.hpp>

#include "docs/yaml/api.hpp"

#include "../../../../../views/contract/base_handler.hpp"
#include "../../../../../waiting/contract/managers/tasks_manager.hpp"

namespace views::waiting::v1::task::update::post {

class WaitingV1TaskUpdatePost final : public views::contract::BaseHandler< ::waiting::handlers::UpdateTaskRequest> {
public:
    static constexpr std::string_view kName = "waiting-v1-task-update-post";

    WaitingV1TaskUpdatePost(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context
    );

protected:
    views::contract::models::ApiResponse Handle(
        ::waiting::handlers::UpdateTaskRequest&& request,
        userver::server::request::RequestContext&& context
    ) const override;

private:
    const ::waiting::contract::managers::TasksManager& tasks_manager_;
};

}  // namespace views::waiting::v1::task::update::post
