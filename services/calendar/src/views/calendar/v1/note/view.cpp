#include "view.hpp"

#include <userver/components/component.hpp>

#include "docs/yaml/api.hpp"

namespace views::calendar::v1::note::post {

CalendarV1NotePost::CalendarV1NotePost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::calendar::handlers::CreateNoteRequest>(config, component_context),
      calendar_manager_(component_context.FindComponent<::calendar::contract::managers::CalendarManager>()) {}

views::contract::models::ApiResponse
CalendarV1NotePost::Handle(::calendar::handlers::CreateNoteRequest&& request, userver::server::request::RequestContext&&)
    const {
    calendar_manager_.CreateNote(std::move(request));

    return contract::models::ApiResponseFactory::Created();
}

}  // namespace views::calendar::v1::note::post
