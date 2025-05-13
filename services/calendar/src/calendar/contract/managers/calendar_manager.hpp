#pragma once

#include <userver/components/component_base.hpp>

#include "../../providers/calendar_provider/calendar_provider.hpp"
#include "../models/calendar_provider_structures.hpp"

#include "docs/yaml/api_fwd.hpp"

namespace calendar::contract::managers {

class CalendarManager final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "calendar-manager";

    CalendarManager(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context
    );

    models::Note GetNote(handlers::NoteIdRequest&& note_id_request) const;

    void CreateNote(handlers::CreateNoteRequest&& create_note_request) const;

    void UpdateNote(handlers::UpdateNoteRequest&& update_note_request) const;

    void DeleteNote(handlers::NoteIdRequest&& note_id_request) const;

    handlers::ListCalendarResponse ListCalendar(handlers::ListCalendarRequest&& list_note_request) const;

private:
    models::NoteForCreate Transform(handlers::CreateNoteRequest&& create_note_request) const;
    models::NoteId Transform(handlers::NoteIdRequest&& note_id_request) const;
    handlers::ListCalendarResponse Transform(std::vector<models::Note>&& calendar, std::optional<std::string>&& cursor) const;
    models::NoteForUpdate Transform(handlers::UpdateNoteRequest&& update_note_request) const;

private:
    const providers::calendar_provider::CalendarProvider& calendar_provider_;
};

}  // namespace calendar::contract::managers
