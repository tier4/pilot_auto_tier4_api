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

#include "air_condition.hpp"

#include <memory>

namespace tier4_vehicle_device_simulator
{

AirCondition::AirCondition(const rclcpp::NodeOptions & options) : Node("air_condition", options)
{
  using std::placeholders::_1;
  using std::placeholders::_2;

  status.enabled = false;
  status.temperature_mode = AirConditionStatus::TEMPERATURE_UNKNOWN;
  status.temperature = 0.0;

  srv_ = create_service<SetAirCondition>(
    "~/air_condition/command", std::bind(&AirCondition::on_command, this, _1, _2));

  pub_ = create_publisher<AirConditionStatus>("~/air_condition/status", rclcpp::QoS(1));

  const auto period = rclcpp::Rate(1.0).period();
  timer_ = rclcpp::create_timer(this, get_clock(), period, [this]() { on_timer(); });
}

void AirCondition::on_command(
  const SetAirCondition::Request::SharedPtr req, const SetAirCondition::Response::SharedPtr res)
{
  status.enabled = req->enabled;
  status.temperature_mode = req->temperature_mode;
  status.temperature = req->temperature;
  res->status.code = 0;
}

void AirCondition::on_timer()
{
  status.stamp = now();
  pub_->publish(status);
}

}  // namespace tier4_vehicle_device_simulator

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(tier4_vehicle_device_simulator::AirCondition)
