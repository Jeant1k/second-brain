#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

#include "calendar/contract/managers/calendar_manager.hpp"
#include "calendar/providers/calendar_provider/calendar_provider.hpp"
#include "views/calendar/v1/note/list/view.hpp"
#include "views/calendar/v1/note/remove/view.hpp"
#include "views/calendar/v1/note/update/view.hpp"
#include "views/calendar/v1/note/view.hpp"

int main(int argc, char* argv[]) {
    auto component_list = userver::components::MinimalServerComponentList()
                              .Append<userver::server::handlers::Ping>()
                              .Append<userver::components::TestsuiteSupport>()
                              .Append<userver::components::HttpClient>()
                              .Append<userver::clients::dns::Component>()
                              .Append<userver::server::handlers::TestsControl>()
                              .Append<userver::components::Postgres>("postgres-calendar")

                              // views
                              .Append<views::calendar::v1::note::post::CalendarV1NotePost>()
                              .Append<views::calendar::v1::note::list::post::CalendarV1NoteListPost>()
                              .Append<views::calendar::v1::note::update::post::CalendarV1NoteUpdatePost>()
                              .Append<views::calendar::v1::note::remove::post::CalendarV1NoteRemovePost>()

                              // calendar
                              .Append<calendar::contract::managers::CalendarManager>()
                              .Append<calendar::providers::calendar_provider::CalendarProvider>();

    return userver::utils::DaemonMain(argc, argv, component_list);
}
