#include "view.hpp"

#include <userver/components/component.hpp>

#include "docs/yaml/api.hpp"

#include "../../../../../notes/contract/models/exceptions.hpp"

namespace views::notes::v1::note::update::post {

NotesV1NoteUpdatePost::NotesV1NoteUpdatePost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::notes::handlers::UpdateNoteRequest>(config, component_context),
      notes_manager_(component_context.FindComponent<::notes::contract::managers::NotesManager>()) {}

views::contract::models::ApiResponse NotesV1NoteUpdatePost::
    Handle(::notes::handlers::UpdateNoteRequest&& request, userver::server::request::RequestContext&&) const {
    try {
        notes_manager_.UpdateNote(std::move(request));
    } catch (const ::notes::contract::models::NoFieldsProvidedException& ex) {
        return contract::models::ApiResponseFactory::BadRequest(
            fmt::format("An error occurred while processing the request: {}", ex.what())
        );
    } catch (const ::notes::contract::models::NoteNotFoundException& ex) {
        return contract::models::ApiResponseFactory::NotFound(
            fmt::format("An error occurred while processing the request: {}", ex.what())
        );
    }

    return contract::models::ApiResponseFactory::Accepted();
}

}  // namespace views::notes::v1::note::update::post
