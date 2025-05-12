#pragma once

#include <userver/components/component_list.hpp>

#include "docs/yaml/api.hpp"

#include "../../../../../notes/contract/managers/tasks_manager.hpp"
#include "../../../../../views/contract/base_handler.hpp"

namespace views::notes::v1::task::remove::post {

class NotesV1TaskRemovePost final : public views::contract::BaseHandler< ::notes::handlers::TaskIdRequest> {
public:
    static constexpr std::string_view kName = "notes-v1-task-remove-post";

    NotesV1TaskRemovePost(
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

}  // namespace views::notes::v1::task::remove::post
