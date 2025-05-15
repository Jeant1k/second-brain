#pragma once

#include <userver/components/component_list.hpp>

#include "docs/yaml/api/internal.hpp"

#include "../../../../../../sometime_later/contract/managers/tasks_manager.hpp"
#include "../../../../../../views/contract/base_handler.hpp"

namespace views::internal::sometime_later::v1::task::move::post {

class InternalSometimeLaterV1TaskMovePost final
    : public views::contract::BaseHandler< ::sometime_later::handlers::MoveTaskRequest> {
public:
    static constexpr std::string_view kName = "internal-sometime-later-v1-task-move-post";

    InternalSometimeLaterV1TaskMovePost(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context
    );

protected:
    views::contract::models::ApiResponse Handle(
        ::sometime_later::handlers::MoveTaskRequest&& request,
        userver::server::request::RequestContext&& context
    ) const override;

private:
    const ::sometime_later::contract::managers::TasksManager& tasks_manager_;
};

}  // namespace views::internal::sometime_later::v1::task::move::post
