#pragma once

#include <userver/components/component_list.hpp>

#include "docs/yaml/internal.hpp"

#include "../../../../../../views/contract/base_handler.hpp"
#include "../../../../../../waiting/contract/managers/tasks_manager.hpp"

namespace views::internal::waiting::v1::task::move::post {

class InternalWaitingV1TaskMovePost final : public views::contract::BaseHandler< ::waiting::handlers::MoveTaskRequest> {
public:
    static constexpr std::string_view kName = "internal-waiting-v1-task-move-post";

    InternalWaitingV1TaskMovePost(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context
    );

protected:
    views::contract::models::ApiResponse Handle(
        ::waiting::handlers::MoveTaskRequest&& request,
        userver::server::request::RequestContext&& context
    ) const override;

private:
    const ::waiting::contract::managers::TasksManager& tasks_manager_;
};

}  // namespace views::internal::waiting::v1::task::move::post
