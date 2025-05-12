#pragma once

#include <userver/components/component_base.hpp>

#include "../../providers/notes_provider/notes_provider.hpp"
#include "../models/notes_provider_structures.hpp"

#include "docs/yaml/api_fwd.hpp"
#include "docs/yaml/internal_fwd.hpp"

namespace notes::contract::managers {

class NotesManager final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "notes-manager";

    NotesManager(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context
    );

    models::Note GetNote(handlers::NoteIdRequest&& note_id_request) const;

    void CreateNote(handlers::CreateNoteRequest&& create_note_request) const;

    void UpdateNote(handlers::UpdateNoteRequest&& update_note_request) const;

    void DeleteNote(handlers::NoteIdRequest&& note_id_request) const;

    handlers::ListNotesResponse ListNotes(handlers::ListNotesRequest&& list_note_request) const;

private:
    models::NoteForCreate Transform(handlers::CreateNoteRequest&& create_note_request) const;
    models::NoteId Transform(handlers::NoteIdRequest&& note_id_request) const;
    handlers::ListNotesResponse Transform(std::vector<models::Note>&& notes, std::optional<std::string>&& cursor) const;
    models::NoteForUpdate Transform(handlers::UpdateNoteRequest&& update_note_request) const;

private:
    const providers::notes_provider::NotesProvider& notes_provider_;
};

}  // namespace notes::contract::managers
