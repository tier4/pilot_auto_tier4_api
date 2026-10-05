# Copyright 2021 Tier IV, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import launch
from launch.actions import DeclareLaunchArgument
from launch.actions import IncludeLaunchDescription
from launch.actions import OpaqueFunction
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitution import Substitution
from launch.substitutions import LaunchConfiguration
from launch.substitutions import PathJoinSubstitution
from launch_ros.actions import ComposableNodeContainer
from launch_ros.actions import LoadComposableNodes
from launch_ros.actions import Node
from launch_ros.descriptions import ComposableNode
from launch_ros.substitutions import FindPackageShare


# Usage: If the current namespace is /ros/ns:
#  - Namespace("/", "foo/bar") -> "ros/ns/foo/bar"
#  - Namespace(".", "foo.bar") -> "ros.ns.foo.bar"
class Namespace(Substitution):
    def __init__(self, separator, suffix):
        super().__init__()
        self.separator = separator
        self.suffix = suffix

    def perform(self, context):
        namespace = context.launch_configurations.get("ros_namespace", "")
        namespace = f"{namespace}{self.separator}{self.suffix}"
        return namespace.replace("/", self.separator).lstrip(self.separator)


def _create_api_node(node_name, class_name, **kwargs):
    return ComposableNode(
        namespace="internal",
        name=node_name,
        package="autoware_iv_internal_api_adaptor",
        plugin="internal_api::" + class_name,
        **kwargs,
    )


def _get_agnocast_env():
    return IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            PathJoinSubstitution(
                [
                    FindPackageShare("autoware_agnocast_wrapper"),
                    "launch",
                    "agnocast_env.launch.py",
                ]
            )
        )
    )


def launch_setup(context, *args, **kwargs):
    use_agnocast = context.perform_substitution(LaunchConfiguration("use_agnocast")) == "1"
    launch_api_0_4_3 = IfCondition(LaunchConfiguration("launch_api_0_4_3")).evaluate(context)

    # IVMsgs is derived from autoware::agnocast_wrapper::Node. Composed like any other node under
    # ENABLE_AGNOCAST=0, where that base is backed by rclcpp; run as its own process under =1.
    iv_msgs_parameters = [{"launch_api_0_4_3": LaunchConfiguration("launch_api_0_4_3")}]
    components = []
    nodes = []
    if use_agnocast:
        nodes.append(
            Node(
                namespace="internal",
                name="iv_msgs",
                package="autoware_iv_internal_api_adaptor",
                executable="iv_msgs_node",
                parameters=iv_msgs_parameters,
                additional_env={"LD_PRELOAD": LaunchConfiguration("ld_preload_value")},
                output="screen",
            )
        )
    else:
        components.append(_create_api_node("iv_msgs", "IVMsgs", parameters=iv_msgs_parameters))

    container = ComposableNodeContainer(
        namespace="internal",
        name="autoware_iv_adaptor",
        package="rclcpp_components",
        executable="component_container_mt",
        composable_node_descriptions=components,
        ros_arguments=[
            "--log-level",
            Namespace(".", "internal.autoware_iv_adaptor:=WARN"),
        ],
        output="screen",
    )
    loader_0_4_3 = LoadComposableNodes(
        target_container=Namespace("/", "internal/autoware_iv_adaptor"),
        condition=IfCondition(LaunchConfiguration("launch_api_0_4_3")),
        composable_node_descriptions=[
            _create_api_node("operator", "Operator"),
            _create_api_node("velocity", "Velocity"),
        ],
    )
    # Under ENABLE_AGNOCAST=1 the container is needed only for the nodes of launch_api_0_4_3.
    if use_agnocast and not launch_api_0_4_3:
        return nodes
    return [container, loader_0_4_3, *nodes]


def generate_launch_description():
    return launch.LaunchDescription(
        [
            DeclareLaunchArgument("launch_api_0_4_3", default_value="false"),
            _get_agnocast_env(),
            OpaqueFunction(function=launch_setup),
        ]
    )
