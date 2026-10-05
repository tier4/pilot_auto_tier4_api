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

#ifndef DRIVING_HPP_
#define DRIVING_HPP_

#include "types.hpp"

#include <autoware/agnocast_wrapper/node.hpp>
#include <rclcpp/rclcpp.hpp>

#include <autoware_adapi_v1_msgs/msg/operation_mode_state.hpp>
#include <autoware_adapi_v1_msgs/srv/change_operation_mode.hpp>
#include <autoware_internal_planning_msgs/msg/velocity_limit.hpp>
#include <autoware_internal_planning_msgs/msg/velocity_limit_clear_command.hpp>
#include <tier4_external_api_msgs/msg/driving_status.hpp>
#include <tier4_external_api_msgs/srv/enable_driving.hpp>

#include <optional>

namespace tier4_monitoring
{

struct LevelAvailable
{
  bool available() const { return route && operators; }
  bool route;
  bool operators;
};

class Driving
{
public:
  explicit Driving(autoware::agnocast_wrapper::Node & node);
  void update_level2_available(bool route, bool operators);
  void update_level4_available(bool route, bool operators);
  void update(const rclcpp::Time & now);
  void publish(const rclcpp::Time & now);

private:
  using OperationModeState = autoware_adapi_v1_msgs::msg::OperationModeState;
  using ChangeOperationMode = autoware_adapi_v1_msgs::srv::ChangeOperationMode;
  using EnableDriving = tier4_external_api_msgs::srv::EnableDriving;
  using DrivingStatus = tier4_external_api_msgs::msg::DrivingStatus;
  using ResponseStatus = tier4_external_api_msgs::msg::ResponseStatus;
  using VelocityLimitSet = autoware_internal_planning_msgs::msg::VelocityLimit;
  using VelocityLimitClear = autoware_internal_planning_msgs::msg::VelocityLimitClearCommand;

  AUTOWARE_SUBSCRIPTION_PTR(OperationModeState) sub_operation_mode_;
  AUTOWARE_CLIENT_PTR(ChangeOperationMode) cli_change_stop_mode;
  AUTOWARE_CLIENT_PTR(ChangeOperationMode) cli_change_autonomous_mode;
  AUTOWARE_PUBLISHER_PTR(DrivingStatus) pub_status_;
  AUTOWARE_SERVICE_PTR(EnableDriving) srv_enable_;
  AUTOWARE_PUBLISHER_PTR(VelocityLimitSet) pub_velocity_limit_set_;
  AUTOWARE_PUBLISHER_PTR(VelocityLimitClear) pub_velocity_limit_clear_;

  void on_operation_mode(const OperationModeState & msg);
  void on_enable(
    const EnableDriving::Request::SharedPtr req, const EnableDriving::Response::SharedPtr res);

  void set_velocity_limit(const rclcpp::Time & now);
  void clear_velocity_limit(const rclcpp::Time & now);

  DrivingLevel current_level_;
  OperationModeState operation_mode_;
  LevelAvailable level2_available;
  LevelAvailable level4_available;
  bool velocity_limit_requested_;

  std::optional<DrivingStatus> prev_status_;
};

}  // namespace tier4_monitoring

#endif  // DRIVING_HPP_
