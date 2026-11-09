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

#include "hazard_alert.hpp"

namespace tier4_autoware_api_extension
{

auto convert(const autoware_internal_control_msgs::msg::HazardSector & internal)
{
  tier4_external_api_msgs::msg::HazardSector external;
  external.start_angle = internal.start_angle;
  external.end_angle = internal.end_angle;
  external.risk_level = internal.risk_level;
  external.sources = internal.sources;
  return external;
}

HazardAlert::HazardAlert(const rclcpp::NodeOptions & options) : Node("hazard_alert", options)
{
  sub_hazard_alert_ = create_subscription<InternalMessage>(
    "/control/control_validator/output/hazard_alert", 1,
    std::bind(&HazardAlert::on_message, this, std::placeholders::_1));

  pub_hazard_alert_ = create_publisher<ExternalMessage>("/api/external/get/hazard_alert", 1);
}

void HazardAlert::on_message(const InternalMessage & internal)
{
  ExternalMessage external;
  external.header = internal.header;
  external.origin = internal.origin;
  for (const auto & sector : internal.sectors) {
    external.sectors.push_back(convert(sector));
  }
  pub_hazard_alert_->publish(external);
}

}  // namespace tier4_autoware_api_extension

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(tier4_autoware_api_extension::HazardAlert)
