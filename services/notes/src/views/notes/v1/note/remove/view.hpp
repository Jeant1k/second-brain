#pragma once

#include <userver/components/component_list.hpp>

#include "docs/yaml/api.hpp"

#include "../../../../../notes/contract/managers/notes_manager.hpp"
#include "../../../../../views/contract/base_handler.hpp"

namespace views::notes::v1::note::remove::post {

class NotesV1NoteRemovePost final : public views::contract::BaseHandler< ::notes::handlers::NoteIdRequest> {
public:
    static constexpr std::string_view kName = "notes-v1-note-remove-post";

    NotesV1NoteRemovePost(
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

}  // namespace views::notes::v1::note::remove::post
