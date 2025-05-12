#pragma once

#include <userver/components/component_list.hpp>

#include "../../../../../notes/contract/managers/notes_manager.hpp"
#include "../../../../../views/contract/base_handler.hpp"
#include "docs/yaml/api.hpp"

namespace views::notes::v1::note::reactivate::post {

class NotesV1NoteReactivatePost final : public views::contract::BaseHandler< ::notes::handlers::NoteIdRequest> {
public:
    static constexpr std::string_view kName = "notes-v1-note-reactivate-post";

    NotesV1NoteReactivatePost(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context
    );

protected:
    views::contract::models::ApiResponse Handle(
        ::notes::handlers::NoteIdRequest&& request,
        userver::server::request::RequestContext&& context
    ) const override;

private:
    const ::notes::contract::managers::NotesManager& notes_manager_;
};

}  // namespace views::notes::v1::note::reactivate::post
