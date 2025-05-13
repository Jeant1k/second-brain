#include "view.hpp"

#include <userver/components/component.hpp>

#include "docs/yaml/api.hpp"

namespace views::notes::v1::note::list::post {

NotesV1NoteListPost::NotesV1NoteListPost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::notes::handlers::ListNotesRequest>(config, component_context),
      notes_manager_(component_context.FindComponent<::notes::contract::managers::NotesManager>()) {}

views::contract::models::ApiResponse
NotesV1NoteListPost::Handle(::notes::handlers::ListNotesRequest&& request, userver::server::request::RequestContext&&)
    const {
    return contract::models::ApiResponseFactory::Ok(notes_manager_.ListNotes(std::move(request)));
}

}  // namespace views::notes::v1::note::list::post
