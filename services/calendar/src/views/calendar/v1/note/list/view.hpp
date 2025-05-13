#pragma once

#include <userver/components/component_list.hpp>

#include "../../../../../calendar/contract/managers/calendar_manager.hpp"
#include "../../../../../views/contract/base_handler.hpp"
#include "docs/yaml/api.hpp"

namespace views::calendar::v1::note::list::post {

class CalendarV1NoteListPost final : public views::contract::BaseHandler< ::calendar::handlers::ListCalendarRequest> {
public:
    static constexpr std::string_view kName = "calendar-v1-note-list-post";

    CalendarV1NoteListPost(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context
    );

protected:
    views::contract::models::ApiResponse Handle(
        ::calendar::handlers::ListCalendarRequest&& request,
        userver::server::request::RequestContext&& context
    ) const override;

private:
    const ::calendar::contract::managers::CalendarManager& calendar_manager_;
};

}  // namespace views::calendar::v1::note::list::post
