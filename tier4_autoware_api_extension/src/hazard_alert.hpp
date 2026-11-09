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

#ifndef HAZARD_ALERT_HPP_
#define HAZARD_ALERT_HPP_

#include <rclcpp/rclcpp.hpp>

#include <autoware_internal_control_msgs/msg/hazard_alert.hpp>
#include <tier4_external_api_msgs/msg/hazard_alert.hpp>

namespace tier4_autoware_api_extension
{

using ExternalMessage = tier4_external_api_msgs::msg::HazardAlert;
using InternalMessage = autoware_internal_control_msgs::msg::HazardAlert;

class HazardAlert : public rclcpp::Node
{
public:
  explicit HazardAlert(const rclcpp::NodeOptions & options);

private:
  void on_message(const InternalMessage & internal);
  rclcpp::Subscription<InternalMessage>::SharedPtr sub_hazard_alert_;
  rclcpp::Publisher<ExternalMessage>::SharedPtr pub_hazard_alert_;
};

}  // namespace tier4_autoware_api_extension

#endif  // HAZARD_ALERT_HPP_
