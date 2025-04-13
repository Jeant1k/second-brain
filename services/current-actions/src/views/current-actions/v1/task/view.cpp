#include "view.hpp"

#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/utils/assert.hpp>

#include <docs/yaml/api.yaml>

namespace views::current_actions::v1::task::post {

CurrentActionsV1TaskPost::CurrentActionsV1TaskPost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
) : userver::server::handlers::HttpHandlerBase(config, component_context) {}

std::string CurrentActionsV1TaskPost::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&
) const {
    request.GetHttpResponse().SetContentType(http::content_type::kApplicationJson);
 
    auto request_json = formats::json::FromString(request.RequestBody());

    auto request_dom = request_json.As<current_actions::handlers::CreateTaskRequest>();

    // request_dom and response_dom have generated types
    auto response_dom = SayHelloTo(request_dom);

    // Use generated serializer for ValueBuilder()
    auto response_json = formats::json::ValueBuilder{response_dom}.ExtractValue();
    return formats::json::ToString(response_json);

}

}  // namespace views::current_actions::v1::task::post
