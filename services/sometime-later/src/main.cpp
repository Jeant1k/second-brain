#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

#include "sometime_later/contract/managers/tasks_manager.hpp"
#include "sometime_later/providers/tasks_provider/tasks_provider.hpp"
#include "views/internal/sometime-later/v1/task/move/view.hpp"
#include "views/sometime-later/v1/task/complete/view.hpp"
#include "views/sometime-later/v1/task/current/actions/view.hpp"
#include "views/sometime-later/v1/task/list/view.hpp"
#include "views/sometime-later/v1/task/pend/view.hpp"
#include "views/sometime-later/v1/task/remove/view.hpp"
#include "views/sometime-later/v1/task/update/view.hpp"
#include "views/sometime-later/v1/task/view.hpp"

int main(int argc, char* argv[]) {
    auto component_list =
        userver::components::MinimalServerComponentList()
            .Append<userver::server::handlers::Ping>()
            .Append<userver::components::TestsuiteSupport>()
            .Append<userver::components::HttpClient>()
            .Append<userver::clients::dns::Component>()
            .Append<userver::server::handlers::TestsControl>()
            .Append<userver::components::Postgres>("postgres-sometime-later")

            // views
            .Append<views::sometime_later::v1::task::post::SometimeLaterV1TaskPost>()
            .Append<views::sometime_later::v1::task::complete::post::SometimeLaterV1TaskCompletePost>()
            .Append<views::sometime_later::v1::task::pend::post::SometimeLaterV1TaskPendPost>()
            .Append<views::sometime_later::v1::task::list::post::SometimeLaterV1TaskListPost>()
            .Append<views::sometime_later::v1::task::update::post::SometimeLaterV1TaskUpdatePost>()
            .Append<views::sometime_later::v1::task::remove::post::SometimeLaterV1TaskRemovePost>()
            .Append<views::sometime_later::v1::task::current::actions::post::SometimeLaterV1TaskCurrentActionsPost>()
            .Append<views::internal::sometime_later::v1::task::move::post::InternalSometimeLaterV1TaskMovePost>()

            // sometime_later
            .Append<sometime_later::contract::managers::TasksManager>()
            .Append<sometime_later::providers::tasks_provider::TasksProvider>();

    return userver::utils::DaemonMain(argc, argv, component_list);
}
