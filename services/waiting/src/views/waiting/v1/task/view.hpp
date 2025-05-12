#pragma once

#include <userver/components/component_list.hpp>

#include "../../../../views/contract/base_handler.hpp"
#include "../../../../waiting/contract/managers/tasks_manager.hpp"
#include "docs/yaml/api.hpp"

namespace views::waiting::v1::task::post {

class WaitingV1TaskPost final : public views::contract::BaseHandler< ::waiting::handlers::CreateTaskRequest> {
public:
    static constexpr std::string_view kName = "waiting-v1-task-post";

    WaitingV1TaskPost(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context
    );

protected:
    views::contract::models::ApiResponse Handle(
        ::waiting::handlers::CreateTaskRequest&& request,
        userver::server::request::RequestContext&& context
    ) const override;

private:
    const ::waiting::contract::managers::TasksManager& tasks_manager_;
};

}  // namespace views::waiting::v1::task::post
