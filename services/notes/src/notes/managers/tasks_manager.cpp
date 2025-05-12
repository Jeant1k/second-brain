#include "../contract/managers/notes_manager.hpp"

#include <userver/components/component_context.hpp>

#include "docs/yaml/api.hpp"
#include "docs/yaml/internal.hpp"

#include "../contract/models/exceptions.hpp"

namespace notes::contract::managers {

namespace {

using models::Note;
using models::NoteForCreate;
using models::NoteForUpdate;
using models::NoteId;
using models::UserId;
using providers::notes_provider::NotesProvider;

}  // namespace

NotesManager::NotesManager(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : userver::components::ComponentBase(config, component_context),
      notes_provider_(component_context.FindComponent<NotesProvider>()) {}

void NotesManager::CreateNote(handlers::CreateNoteRequest&& note) const {
    notes_provider_.InsertNote(Transform(std::move(note)));
}

void NotesManager::UpdateNote(handlers::UpdateNoteRequest&& update_note_request) const {
    if (!update_note_request.name.has_value() && !update_note_request.description.has_value()) {
        throw models::NoFieldsProvidedException{"No note fields provided for update"};
    }

    const auto result = notes_provider_.UpdateNoteFields(Transform(std::move(update_note_request)));

    if (result == NotesProvider::UpdateNoteFieldsResult::kNoteNotFound) {
        throw models::NoteNotFoundException{"Note to update fields was not found"};
    }
}

void NotesManager::DeleteNote(handlers::NoteIdRequest&& note_id_request) const {
    const auto result = notes_provider_.MarkNoteAsDeleted(Transform(std::move(note_id_request)));

    if (result == NotesProvider::MarkNoteAsDeletedResult::kNoteNotFound) {
        throw models::NoteNotFoundException{"Note to mark as deleted was not found"};
    }
}

handlers::ListNotesResponse NotesManager::ListNotes(handlers::ListNotesRequest&& list_note_request) const {
    auto [cursor, notes] = notes_provider_.SelectNotes(
        UserId{list_note_request.user_id},
        models::DeserializeCursorFromString(list_note_request.cursor),
        models::Transform(list_note_request.status)
    );

    return Transform(std::move(notes), SerializeCursorToString(cursor));
}

models::Note NotesManager::GetNote(handlers::NoteIdRequest&& note_id_request) const {
    const auto result = notes_provider_.SelectNoteById(Transform(std::move(note_id_request)));
    if (result.status == NotesProvider::SelectNoteByIdResult::SelectNoteByIdStatus::kNoteNotFound ||
        !result.note.has_value()) {
        throw models::NoteNotFoundException{"Note was not found"};
    }

    return result.note.value();
}

NoteForCreate NotesManager::Transform(handlers::CreateNoteRequest&& create_note_request) const {
    return {
        UserId{create_note_request.user_id},
        std::move(create_note_request.name),
        std::move(create_note_request.description),
    };
}

NoteId NotesManager::Transform(handlers::NoteIdRequest&& note_id_request) const {
    return NoteId{std::move(note_id_request.note_id)};
}

handlers::ListNotesResponse NotesManager::Transform(std::vector<Note>&& notes, std::optional<std::string>&& cursor)
    const {
    using userver::utils::datetime::TimePointTz;

    std::vector<notes::handlers::Note> result_notes;
    result_notes.reserve(notes.size());

    for (auto&& note : notes) {
        result_notes.emplace_back(notes::handlers::Note{
            std::move(note.id.GetUnderlying()),
            std::move(note.user_id.GetUnderlying()),
            std::move(note.name),
            std::move(note.description),
            models::Transform(note.status).value(),
            TimePointTz(note.created_at),
            TimePointTz{note.updated_at},
            note.completed_at.has_value() ? std::make_optional(TimePointTz{note.completed_at.value()}) : std::nullopt
        });
    }

    return {std::move(result_notes), std::move(cursor)};
}

NoteForUpdate NotesManager::Transform(handlers::UpdateNoteRequest&& update_note_request) const {
    return {
        NoteId{update_note_request.note_id},
        std::move(update_note_request.name),
        std::move(update_note_request.description)
    };
}

}  // namespace notes::contract::managers