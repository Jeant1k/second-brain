#include "../contract/managers/calendar_manager.hpp"

#include <userver/components/component_context.hpp>

#include "docs/yaml/api.hpp"

#include "../contract/models/exceptions.hpp"

namespace calendar::contract::managers {

namespace {

using models::Note;
using models::NoteForCreate;
using models::NoteForUpdate;
using models::NoteId;
using models::UserId;
using providers::calendar_provider::CalendarProvider;

}  // namespace

CalendarManager::CalendarManager(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : userver::components::ComponentBase(config, component_context),
      calendar_provider_(component_context.FindComponent<CalendarProvider>()) {}

void CalendarManager::CreateNote(handlers::CreateNoteRequest&& note) const {
    calendar_provider_.InsertNote(Transform(std::move(note)));
}

void CalendarManager::UpdateNote(handlers::UpdateNoteRequest&& update_note_request) const {
    if (!update_note_request.name.has_value() && !update_note_request.description.has_value()) {
        throw models::NoFieldsProvidedException{"No note fields provided for update"};
    }

    const auto result = calendar_provider_.UpdateNoteFields(Transform(std::move(update_note_request)));

    if (result == CalendarProvider::UpdateNoteFieldsResult::kNoteNotFound) {
        throw models::NoteNotFoundException{"Note to update fields was not found"};
    }
}

void CalendarManager::DeleteNote(handlers::NoteIdRequest&& note_id_request) const {
    const auto result = calendar_provider_.MarkNoteAsDeleted(Transform(std::move(note_id_request)));

    if (result == CalendarProvider::MarkNoteAsDeletedResult::kNoteNotFound) {
        throw models::NoteNotFoundException{"Note to mark as deleted was not found"};
    }
}

handlers::ListCalendarResponse CalendarManager::ListCalendar(handlers::ListCalendarRequest&& list_note_request) const {
    auto [cursor, calendar] = calendar_provider_.SelectCalendar(
        UserId{list_note_request.user_id},
        models::DeserializeCursorFromString(list_note_request.cursor),
        models::Transform(list_note_request.status)
    );

    return Transform(std::move(calendar), SerializeCursorToString(cursor));
}

models::Note CalendarManager::GetNote(handlers::NoteIdRequest&& note_id_request) const {
    const auto result = calendar_provider_.SelectNoteById(Transform(std::move(note_id_request)));
    if (result.status == CalendarProvider::SelectNoteByIdResult::SelectNoteByIdStatus::kNoteNotFound ||
        !result.note.has_value()) {
        throw models::NoteNotFoundException{"Note was not found"};
    }

    return result.note.value();
}

NoteForCreate CalendarManager::Transform(handlers::CreateNoteRequest&& create_note_request) const {
    return {
        UserId{create_note_request.user_id},
        std::move(create_note_request.name),
        std::move(create_note_request.description),
    };
}

NoteId CalendarManager::Transform(handlers::NoteIdRequest&& note_id_request) const {
    return NoteId{std::move(note_id_request.note_id)};
}

handlers::ListCalendarResponse CalendarManager::Transform(std::vector<Note>&& calendar, std::optional<std::string>&& cursor)
    const {
    using userver::utils::datetime::TimePointTz;

    std::vector<calendar::handlers::Note> result_calendar;
    result_calendar.reserve(calendar.size());

    for (auto&& note : calendar) {
        result_calendar.emplace_back(calendar::handlers::Note{
            std::move(note.id.GetUnderlying()),
            std::move(note.user_id.GetUnderlying()),
            std::move(note.name),
            std::move(note.description),
            models::Transform(note.status).value(),
            TimePointTz(note.created_at),
            TimePointTz{note.updated_at}
        });
    }

    return {std::move(result_calendar), std::move(cursor)};
}

NoteForUpdate CalendarManager::Transform(handlers::UpdateNoteRequest&& update_note_request) const {
    return {
        NoteId{update_note_request.note_id},
        std::move(update_note_request.name),
        std::move(update_note_request.description)
    };
}

}  // namespace calendar::contract::managers