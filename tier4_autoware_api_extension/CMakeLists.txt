cmake_minimum_required(VERSION 3.14)
project(tier4_autoware_api_extension)

find_package(autoware_cmake REQUIRED)
autoware_package()

ament_auto_add_library(${PROJECT_NAME} SHARED
  src/route_distance.cpp
  src/traffic_light.cpp
  src/planning_factor.cpp
  src/velocity_limit.cpp
)

autoware_agnocast_wrapper_register_node(${PROJECT_NAME}
  PLUGIN "tier4_autoware_api_extension::RouteDistance"
  EXECUTABLE route_distance_node
  ROS2_EXECUTOR SingleThreadedExecutor
  AGNOCAST_EXECUTOR AgnocastOnlyCallbackIsolatedExecutor
)

autoware_agnocast_wrapper_register_node(${PROJECT_NAME}
  PLUGIN "tier4_autoware_api_extension::TrafficLight"
  EXECUTABLE traffic_light_node
  ROS2_EXECUTOR SingleThreadedExecutor
  AGNOCAST_EXECUTOR AgnocastOnlyCallbackIsolatedExecutor
)

autoware_agnocast_wrapper_register_node(${PROJECT_NAME}
  PLUGIN "tier4_autoware_api_extension::PlanningFactor"
  EXECUTABLE planning_factor_node
  ROS2_EXECUTOR SingleThreadedExecutor
  AGNOCAST_EXECUTOR AgnocastOnlyCallbackIsolatedExecutor
)

autoware_agnocast_wrapper_register_node(${PROJECT_NAME}
  PLUGIN "tier4_autoware_api_extension::VelocityLimit"
  EXECUTABLE velocity_limit_node
  ROS2_EXECUTOR SingleThreadedExecutor
  AGNOCAST_EXECUTOR AgnocastOnlyCallbackIsolatedExecutor
)

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)

  ament_add_gtest(test_validation_utils test/test_validation_utils.cpp)
  target_include_directories(test_validation_utils PRIVATE src)
  ament_target_dependencies(test_validation_utils
    autoware_internal_planning_msgs
    rclcpp
    tier4_external_api_msgs
  )
endif()

ament_auto_package(INSTALL_TO_SHARE config launch)
