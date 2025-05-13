#pragma once

#include <userver/components/component_list.hpp>

#include "docs/yaml/api.hpp"

#include "../../../../../calendar/contract/managers/calendar_manager.hpp"
#include "../../../../../views/contract/base_handler.hpp"

namespace views::calendar::v1::note::update::post {

class CalendarV1NoteUpdatePost final : public views::contract::BaseHandler< ::calendar::handlers::UpdateNoteRequest> {
public:
    static constexpr std::string_view kName = "calendar-v1-note-update-post";

    CalendarV1NoteUpdatePost(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context
    );

protected:
    views::contract::models::ApiResponse Handle(
        ::calendar::handlers::UpdateNoteRequest&& request,
        userver::server::request::RequestContext&& context
    ) const override;

private:
    const ::calendar::contract::managers::CalendarManager& calendar_manager_;
};

}  // namespace views::calendar::v1::note::update::post
