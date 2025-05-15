#pragma once

#include <userver/components/component_list.hpp>

#include "../../../../../sometime_later/contract/managers/tasks_manager.hpp"
#include "../../../../../views/contract/base_handler.hpp"
#include "docs/yaml/api/api.hpp"

namespace views::sometime_later::v1::task::list::post {

class SometimeLaterV1TaskListPost final
    : public views::contract::BaseHandler< ::sometime_later::handlers::ListTasksRequest> {
public:
    static constexpr std::string_view kName = "sometime-later-v1-task-list-post";

    SometimeLaterV1TaskListPost(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context
    );

protected:
    views::contract::models::ApiResponse Handle(
        ::sometime_later::handlers::ListTasksRequest&& request,
        userver::server::request::RequestContext&& context
    ) const override;

private:
    const ::sometime_later::contract::managers::TasksManager& tasks_manager_;
};

}  // namespace views::sometime_later::v1::task::list::post
