#include "view.hpp"

#include <userver/components/component.hpp>

#include "docs/yaml/api.hpp"

namespace views::notes::v1::note::post {

NotesV1NotePost::NotesV1NotePost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::notes::handlers::CreateNoteRequest>(config, component_context),
      notes_manager_(component_context.FindComponent<::notes::contract::managers::NotesManager>()) {}

views::contract::models::ApiResponse
NotesV1NotePost::Handle(::notes::handlers::CreateNoteRequest&& request, userver::server::request::RequestContext&&)
    const {
    notes_manager_.CreateNote(std::move(request));

    return contract::models::ApiResponseFactory::Created();
}

}  // namespace views::notes::v1::note::post
