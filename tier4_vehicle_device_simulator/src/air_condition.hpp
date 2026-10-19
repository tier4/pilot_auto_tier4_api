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

#ifndef AIR_CONDITION_HPP_
#define AIR_CONDITION_HPP_

#include <rclcpp/rclcpp.hpp>

#include <tier4_external_api_msgs/msg/air_condition_status.hpp>
#include <tier4_external_api_msgs/srv/set_air_condition.hpp>

namespace tier4_vehicle_device_simulator
{

class AirCondition : public rclcpp::Node
{
public:
  explicit AirCondition(const rclcpp::NodeOptions & options);

private:
  using SetAirCondition = tier4_external_api_msgs::srv::SetAirCondition;
  using AirConditionStatus = tier4_external_api_msgs::msg::AirConditionStatus;
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Service<SetAirCondition>::SharedPtr srv_;
  rclcpp::Publisher<AirConditionStatus>::SharedPtr pub_;

  void on_timer();
  void on_command(
    const SetAirCondition::Request::SharedPtr req, const SetAirCondition::Response::SharedPtr res);

  AirConditionStatus status;
};

}  // namespace tier4_vehicle_device_simulator

#endif  // AIR_CONDITION_HPP_
