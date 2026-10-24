cmake_minimum_required(VERSION 3.8)
project(ros2_cpp_node)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

add_executable(ros2_cpp_node ros2_cpp_node.cpp)

ament_target_dependencies(ros2_cpp_node rclcpp)

install(TARGETS
  ros2_cpp_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()