cmake_minimum_required(VERSION 3.8)
project(change_covariance_value)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()


set(DEPENDENCIES
  "ament_cmake"
  "rclcpp"
  "sensor_msgs"
  "nav_msgs"
  "tf2"
  "geometry_msgs"
  "control_toolbox"
)

set(EXECUTABLE_NAME "imu_covariance")

foreach(X IN LISTS DEPENDENCIES)
    find_package(${X} REQUIRED)
endforeach()


# find_package(ament_cmake REQUIRED)
# find_package(rclcpp REQUIRED)
# find_package(sensor_msgs REQUIRED)


add_executable(${EXECUTABLE_NAME} src/imu_covariance.cpp)
ament_target_dependencies(${EXECUTABLE_NAME} ${DEPENDENCIES})


target_include_directories(${EXECUTABLE_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>)
target_compile_features(${EXECUTABLE_NAME} PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17

install(TARGETS ${EXECUTABLE_NAME}
  DESTINATION lib/${PROJECT_NAME})



ament_package()
