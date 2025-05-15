#include "view.hpp"

#include <userver/components/component.hpp>

#include "../../../../../sometime_later/contract/models/exceptions.hpp"
#include "docs/yaml/api/api.hpp"

namespace views::sometime_later::v1::task::complete::post {

SometimeLaterV1TaskCompletePost::SometimeLaterV1TaskCompletePost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::sometime_later::handlers::TaskIdRequest>(config, component_context),
      tasks_manager_(component_context.FindComponent<::sometime_later::contract::managers::TasksManager>()) {}

views::contract::models::ApiResponse SometimeLaterV1TaskCompletePost::
    Handle(::sometime_later::handlers::TaskIdRequest&& request, userver::server::request::RequestContext&&) const {
    try {
        tasks_manager_.CompleteTask(std::move(request));
    } catch (const ::sometime_later::contract::models::TaskNotFoundException& ex) {
        return contract::models::ApiResponseFactory::NotFound(
            fmt::format("An error occurred while processing the request: {}", ex.what())
        );
    }

    return contract::models::ApiResponseFactory::Ok();
}

}  // namespace views::sometime_later::v1::task::complete::post
