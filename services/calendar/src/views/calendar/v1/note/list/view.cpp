#include "view.hpp"

#include <userver/components/component.hpp>

#include "docs/yaml/api.hpp"

namespace views::calendar::v1::note::list::post {

CalendarV1NoteListPost::CalendarV1NoteListPost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::calendar::handlers::ListCalendarRequest>(config, component_context),
      calendar_manager_(component_context.FindComponent<::calendar::contract::managers::CalendarManager>()) {}

views::contract::models::ApiResponse
CalendarV1NoteListPost::Handle(::calendar::handlers::ListCalendarRequest&& request, userver::server::request::RequestContext&&)
    const {
    return contract::models::ApiResponseFactory::Ok(calendar_manager_.ListCalendar(std::move(request)));
}

}  // namespace views::calendar::v1::note::list::post
