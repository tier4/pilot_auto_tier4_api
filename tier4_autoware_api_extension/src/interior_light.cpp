// Copyright 2026 TIER IV, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "interior_light.hpp"

#include <tier4_api_utils/types/response.hpp>

namespace tier4_autoware_api_extension
{

InteriorLight::InteriorLight(const rclcpp::NodeOptions & options) : Node("interior_light", options)
{
  using std::placeholders::_1;
  using std::placeholders::_2;
  tier4_api_utils::ServiceProxyNodeInterface proxy(this);
  group_ = create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);

  srv_set_ = proxy.create_service<SetService>(
    "/api/external/set/interior_light", std::bind(&InteriorLight::on_set, this, _1, _2),
    rmw_qos_profile_services_default, group_);
  cli_set_ = proxy.create_client<SetService>(
    "/vehicle/interior_light/command", rmw_qos_profile_services_default);
  pub_status_ = create_publisher<StatusMessage>("/api/external/get/interior_light", rclcpp::QoS(1));
  sub_status_ = create_subscription<StatusMessage>(
    "/vehicle/status/interior_light", rclcpp::QoS(1),
    std::bind(&InteriorLight::on_status, this, _1));
}

void InteriorLight::on_set(
  const SetService::Request::SharedPtr request, const SetService::Response::SharedPtr response)
{
  const auto [status, resp] = cli_set_->call(request);
  if (!tier4_api_utils::is_success(status)) {
    response->status = status;
    return;
  }
  response->status = resp->status;
}

void InteriorLight::on_status(const StatusMessage::SharedPtr msg)
{
  pub_status_->publish(*msg);
}

}  // namespace tier4_autoware_api_extension

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(tier4_autoware_api_extension::InteriorLight)
