#pragma once

#include <userver/components/component_list.hpp>

#include "../../../../../notes/contract/managers/tasks_manager.hpp"
#include "../../../../../views/contract/base_handler.hpp"
#include "docs/yaml/api.hpp"

namespace views::notes::v1::task::complete::post {

class NotesV1TaskCompletePost final : public views::contract::BaseHandler< ::notes::handlers::TaskIdRequest> {
public:
    static constexpr std::string_view kName = "notes-v1-task-complete-post";

    NotesV1TaskCompletePost(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context
    );

protected:
    views::contract::models::ApiResponse Handle(
        ::notes::handlers::TaskIdRequest&& request,
        userver::server::request::RequestContext&& context
    ) const override;

private:
    const ::notes::contract::managers::TasksManager& tasks_manager_;
};

}  // namespace views::notes::v1::task::complete::post
