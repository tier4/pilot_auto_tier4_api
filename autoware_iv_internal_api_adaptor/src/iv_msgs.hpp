// Copyright 2021 TIER IV, Inc.
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

#ifndef IV_MSGS_HPP_
#define IV_MSGS_HPP_

#include <autoware/agnocast_wrapper/node.hpp>
#include <rclcpp/rclcpp.hpp>

#include <autoware_adapi_v1_msgs/msg/mrm_state.hpp>
#include <autoware_perception_msgs/msg/tracked_objects.hpp>
#include <autoware_planning_msgs/msg/trajectory.hpp>
#include <autoware_system_msgs/msg/autoware_state.hpp>
#include <autoware_vehicle_msgs/msg/control_mode_report.hpp>
#include <tier4_perception_msgs/msg/dynamic_object_array.hpp>
#include <tier4_planning_msgs/msg/trajectory.hpp>
#include <tier4_system_msgs/msg/autoware_state.hpp>

namespace internal_api
{
class IVMsgs : public autoware::agnocast_wrapper::Node
{
public:
  explicit IVMsgs(const rclcpp::NodeOptions & options);

private:
  using EmergencyStateInput = autoware_adapi_v1_msgs::msg::MrmState;
  using AutowareStateInput = autoware_system_msgs::msg::AutowareState;
  using AutowareStateOutput = tier4_system_msgs::msg::AutowareState;
  AUTOWARE_SUBSCRIPTION_PTR(EmergencyStateInput) sub_emergency_;
  AUTOWARE_SUBSCRIPTION_PTR(AutowareStateInput) sub_state_;
  AUTOWARE_PUBLISHER_PTR(AutowareStateOutput) pub_state_;

  using TrajectoryInput = autoware_planning_msgs::msg::Trajectory;
  using TrajectoryOutput = tier4_planning_msgs::msg::Trajectory;
  AUTOWARE_SUBSCRIPTION_PTR(TrajectoryInput) sub_trajectory_;
  AUTOWARE_PUBLISHER_PTR(TrajectoryOutput) pub_trajectory_;

  using TrackedObjectsInput = autoware_perception_msgs::msg::TrackedObjects;
  using DynamicObjectsOutput = tier4_perception_msgs::msg::DynamicObjectArray;
  AUTOWARE_SUBSCRIPTION_PTR(TrackedObjectsInput) sub_tracked_objects_;
  AUTOWARE_PUBLISHER_PTR(DynamicObjectsOutput) pub_dynamic_objects_;

  void onState(const AutowareStateInput::ConstSharedPtr message);
  void onEmergency(const EmergencyStateInput::ConstSharedPtr message);
  void onTrajectory(const TrajectoryInput::ConstSharedPtr message);
  void onTrackedObjects(const TrackedObjectsInput::ConstSharedPtr message);

  bool is_emergency_;
};

}  // namespace internal_api

#endif  // IV_MSGS_HPP_
