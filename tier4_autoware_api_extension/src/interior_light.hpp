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

#ifndef INTERIOR_LIGHT_HPP_
#define INTERIOR_LIGHT_HPP_

#include <rclcpp/rclcpp.hpp>
#include <tier4_api_utils/tier4_api_utils.hpp>

#include <tier4_external_api_msgs/msg/interior_light_status.hpp>
#include <tier4_external_api_msgs/srv/set_interior_light.hpp>

namespace tier4_autoware_api_extension
{

class InteriorLight : public rclcpp::Node
{
public:
  explicit InteriorLight(const rclcpp::NodeOptions & options);

private:
  using SetService = tier4_external_api_msgs::srv::SetInteriorLight;
  using StatusMessage = tier4_external_api_msgs::msg::InteriorLightStatus;

  rclcpp::CallbackGroup::SharedPtr group_;
  tier4_api_utils::Service<SetService>::SharedPtr srv_set_;
  tier4_api_utils::Client<SetService>::SharedPtr cli_set_;
  rclcpp::Publisher<StatusMessage>::SharedPtr pub_status_;
  rclcpp::Subscription<StatusMessage>::SharedPtr sub_status_;

  void on_set(
    const SetService::Request::SharedPtr request, const SetService::Response::SharedPtr response);
  void on_status(const StatusMessage::SharedPtr msg);
};

}  // namespace tier4_autoware_api_extension

#endif  // INTERIOR_LIGHT_HPP_
