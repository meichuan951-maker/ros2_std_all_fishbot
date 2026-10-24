cmake_minimum_required(VERSION 3.5)
project(ddynamic_reconfigure)

# Default to C++14
if (NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif ()

if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif ()

##################
##  Build setup ##
##################

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)

##############
##   Build  ##
##############

include_directories(
  include
)

add_library(${PROJECT_NAME} SHARED src/ddynamic_reconfigure.cpp)
ament_target_dependencies(${PROJECT_NAME} rclcpp rclcpp_lifecycle)
target_link_libraries(${PROJECT_NAME})

add_executable(fake_dynamic_reconfigure_server test/fake_dynamic_reconfigure_server.cpp)
target_link_libraries(fake_dynamic_reconfigure_server ${PROJECT_NAME})

#############
## Install ##
#############

install(
  DIRECTORY include/
  DESTINATION include
)

install(
  TARGETS
  ${PROJECT_NAME}
  fake_dynamic_reconfigure_server
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})
ament_export_dependencies(rclcpp)
ament_export_dependencies(rclcpp_lifecycle)
ament_package()
