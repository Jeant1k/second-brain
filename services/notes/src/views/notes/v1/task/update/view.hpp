#pragma once

#include <userver/components/component_list.hpp>

#include "docs/yaml/api.hpp"

#include "../../../../../notes/contract/managers/notes_manager.hpp"
#include "../../../../../views/contract/base_handler.hpp"

namespace views::notes::v1::note::update::post {

class NotesV1NoteUpdatePost final : public views::contract::BaseHandler< ::notes::handlers::UpdateNoteRequest> {
public:
    static constexpr std::string_view kName = "notes-v1-note-update-post";

    NotesV1NoteUpdatePost(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context
    );

protected:
    views::contract::models::ApiResponse Handle(
        ::notes::handlers::UpdateNoteRequest&& request,
        userver::server::request::RequestContext&& context
    ) const override;

private:
    const ::notes::contract::managers::NotesManager& notes_manager_;
};

}  // namespace views::notes::v1::note::update::post
