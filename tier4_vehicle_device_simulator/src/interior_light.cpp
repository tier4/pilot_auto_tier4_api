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

#include "interior_light.hpp"

#include <memory>

namespace tier4_vehicle_device_simulator
{

InteriorLight::InteriorLight(const rclcpp::NodeOptions & options) : Node("interior_light", options)
{
  using std::placeholders::_1;
  using std::placeholders::_2;

  status.enabled = false;

  srv_ = create_service<SetSwitch>(
    "~/interior_light/command", std::bind(&InteriorLight::on_command, this, _1, _2));

  pub_ = create_publisher<SwitchStatus>("~/interior_light/status", rclcpp::QoS(1));

  const auto period = rclcpp::Rate(1.0).period();
  timer_ = rclcpp::create_timer(this, get_clock(), period, [this]() { on_timer(); });
}

void InteriorLight::on_command(
  const SetSwitch::Request::SharedPtr req, const SetSwitch::Response::SharedPtr res)
{
  status.enabled = req->enabled;
  res->status.code = 0;
}

void InteriorLight::on_timer()
{
  status.stamp = now();
  pub_->publish(status);
}

}  // namespace tier4_vehicle_device_simulator

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(tier4_vehicle_device_simulator::InteriorLight)
