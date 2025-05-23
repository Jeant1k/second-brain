#pragma once

#include <userver/components/component_base.hpp>
#include <userver/storages/postgres/cluster.hpp>

#include "../../contract/models/notes_provider_structures.hpp"

namespace notes::providers::notes_provider {

class NotesProvider final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "notes-provider";

    NotesProvider(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context
    );

    void InsertNote(contract::models::NoteForCreate&& note) const;

    void UpsertNote(contract::models::Note&& note) const;

    std::optional<contract::models::UserId> SelectUserIdByNoteId(contract::models::NoteId&& note_id) const;

    enum class MarkNoteAsCompletedResult : char { kSuccess, kNoteNotFound };
    MarkNoteAsCompletedResult MarkNoteAsCompleted(contract::models::NoteId&& note_id) const;

    enum class MarkNoteAsActiveResult : char { kSuccess, kNoteNotFound };
    MarkNoteAsActiveResult MarkNoteAsActive(contract::models::NoteId&& note_id) const;

    enum class MarkNoteAsDeletedResult : char { kSuccess, kNoteNotFound };
    MarkNoteAsDeletedResult MarkNoteAsDeleted(contract::models::NoteId&& note_id) const;

    void MarkNoteAsMovedToCurrentActions(contract::models::NoteId&& note_id) const;

    struct SelectNoteByIdResult {
        enum class SelectNoteByIdStatus : char { kSuccess, kNoteNotFound };

        SelectNoteByIdStatus status;
        std::optional<contract::models::Note> note;
    };
    SelectNoteByIdResult SelectNoteById(contract::models::NoteId&& note_id) const;

    enum class UpdateNoteFieldsResult : char { kSuccess, kNoteNotFound };
    UpdateNoteFieldsResult UpdateNoteFields(contract::models::NoteForUpdate&& note) const;

    struct SelectNotesResult {
        std::optional<contract::models::Cursor> cursor;
        std::vector<contract::models::Note> notes;
    };
    SelectNotesResult SelectNotes(
        contract::models::UserId&& user_id,
        std::optional<contract::models::Cursor>&& cursor = std::nullopt,
        std::optional<contract::models::Status>&& status = std::nullopt
    ) const;

private:
    const userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace notes::providers::notes_provider
