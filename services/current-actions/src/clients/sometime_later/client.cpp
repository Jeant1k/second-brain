#include "client.hpp"

#include <userver/clients/http/component.hpp>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/yaml_config/merge_schemas.hpp>

#include "docs/yaml/clients/sometime_later/internal.hpp"

namespace clients::sometime_later {

SometimeLaterClient::SometimeLaterClient(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
)
    : userver::components::LoggableComponentBase(config, context),
      contract::BaseClient(
          context.FindComponent<userver::components::HttpClient>().GetHttpClient(),
          config["base-url"].As<std::string>()
      ) {}

void SometimeLaterClient::MoveTask(::sometime_later::handlers::Task&& task) const {
    ::sometime_later::handlers::MoveTaskRequest request_body;
    request_body.task = task;

    PerformRequest<void, ::sometime_later::handlers::MoveTaskRequest>(
        userver::clients::http::HttpMethod::kPost, "/internal/sometime-later/v1/task/move", request_body
    );
}

userver::yaml_config::Schema SometimeLaterClient::GetStaticConfigSchema() {
    return userver::yaml_config::MergeSchemas<userver::components::LoggableComponentBase>(R"(
        type: object
        description: Client for the internal sometime-later service.
        additionalProperties: false
        properties:
            base-url:
                type: string
                description: Base URL of the sometime-later service (e.g., http://sometime-later)
        )");
}

}  // namespace clients::sometime_later