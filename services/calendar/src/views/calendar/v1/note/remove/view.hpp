#pragma once

#include <userver/components/component_list.hpp>

#include "docs/yaml/api.hpp"

#include "../../../../../calendar/contract/managers/calendar_manager.hpp"
#include "../../../../../views/contract/base_handler.hpp"

namespace views::calendar::v1::note::remove::post {

class CalendarV1NoteRemovePost final : public views::contract::BaseHandler< ::calendar::handlers::NoteIdRequest> {
public:
    static constexpr std::string_view kName = "calendar-v1-note-remove-post";

    CalendarV1NoteRemovePost(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context
    );

protected:
    views::contract::models::ApiResponse Handle(
        ::calendar::handlers::NoteIdRequest&& request,
        userver::server::request::RequestContext&& context
    ) const override;

private:
    const ::calendar::contract::managers::CalendarManager& calendar_manager_;
};

}  // namespace views::calendar::v1::note::remove::post
