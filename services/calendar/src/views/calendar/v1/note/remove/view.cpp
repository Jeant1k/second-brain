#include "view.hpp"

#include <userver/components/component.hpp>

#include "../../../../../calendar/contract/models/exceptions.hpp"
#include "docs/yaml/api.hpp"

namespace views::calendar::v1::note::remove::post {

CalendarV1NoteRemovePost::CalendarV1NoteRemovePost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::calendar::handlers::NoteIdRequest>(config, component_context),
      calendar_manager_(component_context.FindComponent<::calendar::contract::managers::CalendarManager>()) {}

views::contract::models::ApiResponse
CalendarV1NoteRemovePost::Handle(::calendar::handlers::NoteIdRequest&& request, userver::server::request::RequestContext&&)
    const {
    try {
        calendar_manager_.DeleteNote(std::move(request));
    } catch (const ::calendar::contract::models::NoteNotFoundException& ex) {
        return contract::models::ApiResponseFactory::NotFound(
            fmt::format("An error occurred while processing the request: {}", ex.what())
        );
    }

    return contract::models::ApiResponseFactory::Ok();
}

}  // namespace views::calendar::v1::note::remove::post
