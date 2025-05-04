#include "client.hpp"

#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/yaml_config/merge_schemas.hpp>
#include <userver/clients/http/component.hpp>

#include "docs/yaml/internal.hpp"

namespace clients::current_actions {

CurrentActionsClient::CurrentActionsClient(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
)
    : userver::components::LoggableComponentBase(config, context),
    contract::BaseClient(
        context.FindComponent<userver::components::HttpClient>().GetHttpClient(),
        config["base-url"].As<std::string>()
    ) {}

void CurrentActionsClient::MoveTask(::sometime_later::handlers::Task&& task) const {
    ::sometime_later::handlers::MoveTaskRequest request_body;
    request_body.task = task;

    PerformRequest<void, ::sometime_later::handlers::MoveTaskRequest>(
        userver::clients::http::HttpMethod::kPost,
        "/internal/current-actions/v1/task/move",
        request_body
    );
}

userver::yaml_config::Schema CurrentActionsClient::GetStaticConfigSchema() {
    return userver::yaml_config::MergeSchemas<userver::components::LoggableComponentBase>(R"(
        type: object
        description: Client for the internal current-actions service.
        additionalProperties: false
        properties:
            base-url:
                type: string
                description: Base URL of the current-actions service (e.g., http://current-actions)
        )"
    );
}

}  // namespace clients::current_actions
