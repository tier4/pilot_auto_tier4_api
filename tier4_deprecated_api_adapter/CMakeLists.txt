cmake_minimum_required(VERSION 3.14)
project(tier4_deprecated_api_adapter)

find_package(autoware_cmake REQUIRED)
autoware_package()

ament_auto_add_library(${PROJECT_NAME} SHARED
  src/hazard_status.cpp
  src/initial_pose.cpp
)

rclcpp_components_register_nodes(${PROJECT_NAME}
  "tier4_deprecated_api_adapter::HazardStatus"
  "tier4_deprecated_api_adapter::InitialPose"
)

ament_auto_package(INSTALL_TO_SHARE launch)
