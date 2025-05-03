#include "view.hpp"

#include <userver/components/component.hpp>

#include "docs/yaml/api.hpp"

namespace views::sometime_later::v1::task::current::actions::post {

SometimeLaterV1TaskCurrentActionsPost::SometimeLaterV1TaskCurrentActionsPost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : views::contract::BaseHandler<::sometime_later::handlers::TaskIdRequest>(config, component_context),
      tasks_manager_(component_context.FindComponent<::sometime_later::contract::managers::TasksManager>()) {}

views::contract::models::ApiResponse SometimeLaterV1TaskCurrentActionsPost::
    Handle(::sometime_later::handlers::TaskIdRequest&& request, userver::server::request::RequestContext&&) const {
    // TODO: сначала нужно брать задачу по ID, потом пытаться переместить в current-actions, а потом помечать
    // перемещенной
    auto task = tasks_manager_.MoveToCurrentActionsTask(std::move(request));

    return contract::models::ApiResponseFactory::Ok();
}

}  // namespace views::sometime_later::v1::task::current::actions::post
