cmake_minimum_required(VERSION 3.14)
project(tier4_vehicle_device_simulator)

find_package(autoware_cmake REQUIRED)
autoware_package()

ament_auto_add_library(${PROJECT_NAME} SHARED
  src/air_condition.cpp
  src/interior_light.cpp
)

rclcpp_components_register_node(${PROJECT_NAME}
  PLUGIN "tier4_vehicle_device_simulator::AirCondition"
  EXECUTABLE air_condition_node
)

rclcpp_components_register_node(${PROJECT_NAME}
  PLUGIN "tier4_vehicle_device_simulator::InteriorLight"
  EXECUTABLE interior_light_node
)

ament_auto_package(INSTALL_TO_SHARE launch)
