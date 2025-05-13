#include "view.hpp"

#include <userver/components/component.hpp>

#include "docs/yaml/api.hpp"

#include "../../../../../calendar/contract/models/exceptions.hpp"

namespace views::calendar::v1::note::update::post {

CalendarV1NoteUpdatePost::CalendarV1NoteUpdatePost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::calendar::handlers::UpdateNoteRequest>(config, component_context),
      calendar_manager_(component_context.FindComponent<::calendar::contract::managers::CalendarManager>()) {}

views::contract::models::ApiResponse CalendarV1NoteUpdatePost::
    Handle(::calendar::handlers::UpdateNoteRequest&& request, userver::server::request::RequestContext&&) const {
    try {
        calendar_manager_.UpdateNote(std::move(request));
    } catch (const ::calendar::contract::models::NoFieldsProvidedException& ex) {
        return contract::models::ApiResponseFactory::BadRequest(
            fmt::format("An error occurred while processing the request: {}", ex.what())
        );
    } catch (const ::calendar::contract::models::NoteNotFoundException& ex) {
        return contract::models::ApiResponseFactory::NotFound(
            fmt::format("An error occurred while processing the request: {}", ex.what())
        );
    }

    return contract::models::ApiResponseFactory::Accepted();
}

}  // namespace views::calendar::v1::note::update::post
