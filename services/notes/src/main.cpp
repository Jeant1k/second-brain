#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

#include "notes/contract/managers/tasks_manager.hpp"
#include "notes/providers/tasks_provider/tasks_provider.hpp"
#include "views/notes/v1/task/complete/view.hpp"
#include "views/notes/v1/task/list/view.hpp"
#include "views/notes/v1/task/reactivate/view.hpp"
#include "views/notes/v1/task/remove/view.hpp"
#include "views/notes/v1/task/update/view.hpp"
#include "views/notes/v1/task/view.hpp"

int main(int argc, char* argv[]) {
    auto component_list = userver::components::MinimalServerComponentList()
                              .Append<userver::server::handlers::Ping>()
                              .Append<userver::components::TestsuiteSupport>()
                              .Append<userver::components::HttpClient>()
                              .Append<userver::clients::dns::Component>()
                              .Append<userver::server::handlers::TestsControl>()
                              .Append<userver::components::Postgres>("postgres-notes")

                              // views
                              .Append<views::notes::v1::task::post::NotesV1TaskPost>()
                              .Append<views::notes::v1::task::complete::post::NotesV1TaskCompletePost>()
                              .Append<views::notes::v1::task::reactivate::post::NotesV1TaskReactivatePost>()
                              .Append<views::notes::v1::task::list::post::NotesV1TaskListPost>()
                              .Append<views::notes::v1::task::update::post::NotesV1TaskUpdatePost>()
                              .Append<views::notes::v1::task::remove::post::NotesV1TaskRemovePost>()

                              // notes
                              .Append<notes::contract::managers::TasksManager>()
                              .Append<notes::providers::tasks_provider::TasksProvider>();

    return userver::utils::DaemonMain(argc, argv, component_list);
}
