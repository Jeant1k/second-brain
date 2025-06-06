#include "view.hpp"

#include <userver/components/component.hpp>

#include "../../../../../notes/contract/models/exceptions.hpp"
#include "docs/yaml/api.hpp"

namespace views::notes::v1::note::remove::post {

NotesV1NoteRemovePost::NotesV1NoteRemovePost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::notes::handlers::NoteIdRequest>(config, component_context),
      notes_manager_(component_context.FindComponent<::notes::contract::managers::NotesManager>()) {}

views::contract::models::ApiResponse
NotesV1NoteRemovePost::Handle(::notes::handlers::NoteIdRequest&& request, userver::server::request::RequestContext&&)
    const {
    try {
        notes_manager_.DeleteNote(std::move(request));
    } catch (const ::notes::contract::models::NoteNotFoundException& ex) {
        return contract::models::ApiResponseFactory::NotFound(
            fmt::format("An error occurred while processing the request: {}", ex.what())
        );
    }

    return contract::models::ApiResponseFactory::Ok();
}

}  // namespace views::notes::v1::note::remove::post
