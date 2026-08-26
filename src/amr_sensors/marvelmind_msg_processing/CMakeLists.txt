cmake_minimum_required(VERSION 3.8)
project(marvelmind_msg_processing)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies

set(DEPENDENCIES
  "ament_cmake"
  "rclcpp"
  "tf2"
  "geometry_msgs"
  "marvelmind_ros2_msgs"
  "tf2_ros"
)

foreach(X IN LISTS DEPENDENCIES)
    find_package(${X} REQUIRED)
endforeach()

set(EXECUTABLE_NAME "marvelmind_adapter")
add_executable(${EXECUTABLE_NAME} src/marvelmind_adapter.cpp src/main.cpp)
ament_target_dependencies(${EXECUTABLE_NAME} ${DEPENDENCIES})


target_include_directories(${EXECUTABLE_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>)
target_compile_features(${EXECUTABLE_NAME} PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17

install(TARGETS ${EXECUTABLE_NAME}
  DESTINATION lib/${PROJECT_NAME})

ament_package()
