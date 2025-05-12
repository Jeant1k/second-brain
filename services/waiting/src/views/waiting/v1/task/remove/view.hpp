#pragma once

#include <userver/components/component_list.hpp>

#include "docs/yaml/api.hpp"

#include "../../../../../views/contract/base_handler.hpp"
#include "../../../../../waiting/contract/managers/tasks_manager.hpp"

namespace views::waiting::v1::task::remove::post {

class WaitingV1TaskRemovePost final : public views::contract::BaseHandler< ::waiting::handlers::TaskIdRequest> {
public:
    static constexpr std::string_view kName = "waiting-v1-task-remove-post";

    WaitingV1TaskRemovePost(
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
};

}  // namespace views::waiting::v1::task::remove::post
