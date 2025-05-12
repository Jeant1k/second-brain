#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

#include "views/waiting/v1/task/complete/view.hpp"
#include "views/waiting/v1/task/list/view.hpp"
#include "views/waiting/v1/task/reactivate/view.hpp"
#include "views/waiting/v1/task/remove/view.hpp"
#include "views/waiting/v1/task/update/view.hpp"
#include "views/waiting/v1/task/view.hpp"
#include "waiting/contract/managers/tasks_manager.hpp"
#include "waiting/providers/tasks_provider/tasks_provider.hpp"

int main(int argc, char* argv[]) {
    auto component_list = userver::components::MinimalServerComponentList()
                              .Append<userver::server::handlers::Ping>()
                              .Append<userver::components::TestsuiteSupport>()
                              .Append<userver::components::HttpClient>()
                              .Append<userver::clients::dns::Component>()
                              .Append<userver::server::handlers::TestsControl>()
                              .Append<userver::components::Postgres>("postgres-waiting")

                              // views
                              .Append<views::waiting::v1::task::post::WaitingV1TaskPost>()
                              .Append<views::waiting::v1::task::complete::post::WaitingV1TaskCompletePost>()
                              .Append<views::waiting::v1::task::reactivate::post::WaitingV1TaskReactivatePost>()
                              .Append<views::waiting::v1::task::list::post::WaitingV1TaskListPost>()
                              .Append<views::waiting::v1::task::update::post::WaitingV1TaskUpdatePost>()
                              .Append<views::waiting::v1::task::remove::post::WaitingV1TaskRemovePost>()

                              // waiting
                              .Append<waiting::contract::managers::TasksManager>()
                              .Append<waiting::providers::tasks_provider::TasksProvider>();

    return userver::utils::DaemonMain(argc, argv, component_list);
}
