#include "notes_provider.hpp"

#include <fmt/format.h>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <userver/components/component_context.hpp>
#include <userver/logging/log.hpp>
#include <userver/storages/postgres/component.hpp>

#include "notes/sql_queries.hpp"

namespace notes::providers::notes_provider {

namespace {

using contract::models::Cursor;
using contract::models::Note;
using contract::models::NoteForCreate;
using contract::models::NoteForUpdate;
using contract::models::NoteId;
using contract::models::Status;
using contract::models::UserId;

constexpr std::int32_t kNotesLimit{10};

}  // namespace

NotesProvider::NotesProvider(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : userver::components::ComponentBase(config, component_context),
      pg_cluster_(component_context.FindComponent<userver::components::Postgres>("postgres-notes").GetCluster()) {}

void NotesProvider::InsertNote(NoteForCreate&& note) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        sql::kInsertNote,
        note.user_id.GetUnderlying(),
        note.name,
        note.description
    );

    auto note_id = boost::uuids::to_string((*result.cbegin())["id"].As<boost::uuids::uuid>());

    LOG_INFO() << fmt::format("Note with id = {} was inserted", std::move(note_id));
}

std::optional<UserId> NotesProvider::SelectUserIdByNoteId(NoteId&& note_id) const {
    const auto user_id_opt = pg_cluster_
                                 ->Execute(
                                     userver::storages::postgres::ClusterHostType::kMaster,
                                     sql::kSelectUserIdByNoteId,
                                     note_id.GetUnderlying()
                                 )
                                 .AsOptionalSingleRow<std::int64_t>(userver::storages::postgres::kFieldTag);

    if (!user_id_opt.has_value()) {
        return std::nullopt;
    }

    return UserId{user_id_opt.value()};
}

NotesProvider::MarkNoteAsDeletedResult NotesProvider::MarkNoteAsDeleted(NoteId&& note_id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster, sql::kMarkNoteAsDeleted, note_id.GetUnderlying()
    );

    if (result.RowsAffected() == 0) {
        LOG_WARNING() << fmt::format(
            "Note with id = {} was not marked as deleted", boost::uuids::to_string(note_id.GetUnderlying())
        );
        return MarkNoteAsDeletedResult::kNoteNotFound;
    }

    LOG_INFO(
    ) << fmt::format("Note with id = {} was marked as deleted", boost::uuids::to_string(note_id.GetUnderlying()));
    return MarkNoteAsDeletedResult::kSuccess;
}

NotesProvider::SelectNoteByIdResult NotesProvider::SelectNoteById(contract::models::NoteId&& note_id) const {
    auto note =
        pg_cluster_
            ->Execute(
                userver::storages::postgres::ClusterHostType::kMaster, sql::kSelectNoteById, note_id.GetUnderlying()
            )
            .AsOptionalSingleRow<contract::models::Note>(userver::storages::postgres::kRowTag);

    if (!note.has_value()) {
        LOG_WARNING(
        ) << fmt::format("Note with id = {} was not found", boost::uuids::to_string(note_id.GetUnderlying()));
        return {SelectNoteByIdResult::SelectNoteByIdStatus::kNoteNotFound, std::nullopt};
    }

    return {SelectNoteByIdResult::SelectNoteByIdStatus::kSuccess, std::move(note)};
}

NotesProvider::UpdateNoteFieldsResult NotesProvider::UpdateNoteFields(NoteForUpdate&& note) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        sql::kUpdateNameOrDescription,
        note.note_id.GetUnderlying(),
        note.name,
        note.description
    );

    if (result.RowsAffected() == 0) {
        LOG_WARNING(
        ) << fmt::format("No note with id {} found for update", boost::uuids::to_string(note.note_id.GetUnderlying()));
        return UpdateNoteFieldsResult::kNoteNotFound;
    }

    LOG_INFO() << fmt::format("Note with id {} updated fields", boost::uuids::to_string(note.note_id.GetUnderlying()));
    return UpdateNoteFieldsResult::kSuccess;
}

NotesProvider::SelectNotesResult
NotesProvider::SelectNotes(UserId&& user_id, std::optional<Cursor>&& cursor, std::optional<Status>&& status) const {
    std::optional<userver::storages::postgres::TimePointTz> updated_at;
    std::optional<boost::uuids::uuid> note_id;
    if (cursor.has_value()) {
        updated_at = cursor.value().updated_at;
        note_id = std::move(cursor.value().id.GetUnderlying());
    }

    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        sql::kSelectNotes,
        user_id.GetUnderlying(),
        updated_at,
        note_id,
        status,
        kNotesLimit + 1
    );

    SelectNotesResult select_notes_result;
    select_notes_result.notes = result.AsContainer<std::vector<Note>>(userver::storages::postgres::kRowTag);

    if (select_notes_result.notes.size() > kNotesLimit) {
        auto& last_note = select_notes_result.notes.back();
        select_notes_result.cursor = {last_note.updated_at, last_note.id};

        select_notes_result.notes.resize(kNotesLimit);
    }

    LOG_INFO() << fmt::format("Selected {} notes for user_id = {}", select_notes_result.notes.size(), user_id);

    return select_notes_result;
}

}  // namespace notes::providers::notes_provider