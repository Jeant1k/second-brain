#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

#include "notes/contract/managers/notes_manager.hpp"
#include "notes/providers/notes_provider/notes_provider.hpp"
#include "views/notes/v1/note/list/view.hpp"
#include "views/notes/v1/note/remove/view.hpp"
#include "views/notes/v1/note/update/view.hpp"
#include "views/notes/v1/note/view.hpp"

int main(int argc, char* argv[]) {
    auto component_list = userver::components::MinimalServerComponentList()
                              .Append<userver::server::handlers::Ping>()
                              .Append<userver::components::TestsuiteSupport>()
                              .Append<userver::components::HttpClient>()
                              .Append<userver::clients::dns::Component>()
                              .Append<userver::server::handlers::TestsControl>()
                              .Append<userver::components::Postgres>("postgres-notes")

                              // views
                              .Append<views::notes::v1::note::post::NotesV1NotePost>()
                              .Append<views::notes::v1::note::list::post::NotesV1NoteListPost>()
                              .Append<views::notes::v1::note::update::post::NotesV1NoteUpdatePost>()
                              .Append<views::notes::v1::note::remove::post::NotesV1NoteRemovePost>()

                              // notes
                              .Append<notes::contract::managers::NotesManager>()
                              .Append<notes::providers::notes_provider::NotesProvider>();

    return userver::utils::DaemonMain(argc, argv, component_list);
}
