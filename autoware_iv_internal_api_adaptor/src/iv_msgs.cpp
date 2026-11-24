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

#include "iv_msgs.hpp"

#include <tier4_auto_msgs_converter/tier4_auto_msgs_converter.hpp>

namespace internal_api
{
IVMsgs::IVMsgs(const rclcpp::NodeOptions & options) : Node("external_api_iv_msgs", options)
{
  using std::placeholders::_1;

  pub_trajectory_ = create_publisher<TrajectoryOutput>(
    "/api/iv_msgs/planning/scenario_planning/trajectory", rclcpp::QoS(1));
  sub_trajectory_ = create_subscription<TrajectoryInput>(
    "/planning/trajectory", rclcpp::QoS(1), std::bind(&IVMsgs::onTrajectory, this, _1));

  pub_dynamic_objects_ = create_publisher<DynamicObjectsOutput>(
    "/api/iv_msgs/perception/object_recognition/tracking/objects", rclcpp::QoS(1));
  sub_tracked_objects_ = create_subscription<TrackedObjectsInput>(
    "/perception/object_recognition/tracking/objects", rclcpp::QoS(1),
    std::bind(&IVMsgs::onTrackedObjects, this, _1));
}

void IVMsgs::onTrajectory(const TrajectoryInput::ConstSharedPtr message)
{
  pub_trajectory_->publish(tier4_auto_msgs_converter::convert(*message));
}

void IVMsgs::onTrackedObjects(const TrackedObjectsInput::ConstSharedPtr message)
{
  pub_dynamic_objects_->publish(tier4_auto_msgs_converter::convert(*message));
}

}  // namespace internal_api

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(internal_api::IVMsgs)
