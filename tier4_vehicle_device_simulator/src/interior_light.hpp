// Copyright 2026 The Autoware Contributors
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

#include <tier4_external_api_msgs/msg/switch_status.hpp>
#include <tier4_external_api_msgs/srv/set_switch.hpp>

namespace tier4_vehicle_device_simulator
{

class InteriorLight : public rclcpp::Node
{
public:
  explicit InteriorLight(const rclcpp::NodeOptions & options);

private:
  using SetSwitch = tier4_external_api_msgs::srv::SetSwitch;
  using SwitchStatus = tier4_external_api_msgs::msg::SwitchStatus;
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Service<SetSwitch>::SharedPtr srv_;
  rclcpp::Publisher<SwitchStatus>::SharedPtr pub_;

  void on_timer();
  void on_command(
    const SetSwitch::Request::SharedPtr req, const SetSwitch::Response::SharedPtr res);

  SwitchStatus status;
};

}  // namespace tier4_vehicle_device_simulator

#endif  // INTERIOR_LIGHT_HPP_
