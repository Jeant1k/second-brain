#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

#include "current_actions/contract/managers/tasks_manager.hpp"
#include "current_actions/providers/tasks_provider/tasks_provider.hpp"
#include "views/current-actions/v1/task/complete/view.hpp"
#include "views/current-actions/v1/task/list/view.hpp"
#include "views/current-actions/v1/task/reactivate/view.hpp"
#include "views/current-actions/v1/task/view.hpp"

int main(int argc, char* argv[]) {
    auto component_list =
        userver::components::MinimalServerComponentList()
            .Append<userver::server::handlers::Ping>()
            .Append<userver::components::TestsuiteSupport>()
            .Append<userver::components::HttpClient>()
            .Append<userver::clients::dns::Component>()
            .Append<userver::server::handlers::TestsControl>()
            .Append<userver::components::Postgres>("postgres-current-actions")

            // views
            .Append<views::current_actions::v1::task::post::CurrentActionsV1TaskPost>()
            .Append<views::current_actions::v1::task::complete::post::CurrentActionsV1TaskCompletePost>()
            .Append<views::current_actions::v1::task::reactivate::post::CurrentActionsV1TaskReactivatePost>()
            .Append<views::current_actions::v1::task::list::post::CurrentActionsV1TaskListPost>()

            // current_actions
            .Append<current_actions::contract::managers::TasksManager>()
            .Append<current_actions::providers::tasks_provider::TasksProvider>();

    return userver::utils::DaemonMain(argc, argv, component_list);
}
