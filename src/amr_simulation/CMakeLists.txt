cmake_minimum_required(VERSION 3.8)
project(amr_simulation)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

set(DEPENDENCIES
  "rclcpp"
  "geometry_msgs"
  "nav_msgs"
)
set(EXECUTABLE_NAME "convert_pose_to_odometry_node")


add_executable(${EXECUTABLE_NAME} src/convert_pose_to_odometry.cpp)
ament_target_dependencies(${EXECUTABLE_NAME} ${DEPENDENCIES})


target_include_directories(${EXECUTABLE_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>)
target_compile_features(${EXECUTABLE_NAME} PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17

install(TARGETS ${EXECUTABLE_NAME}
  DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY models scenarios worlds launch rviz
  DESTINATION share/${PROJECT_NAME}
)



ament_package()
