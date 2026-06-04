cmake_minimum_required(VERSION 3.8)
project(localization_error_calculator)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()


set(DEPENDENCIES
  "ament_cmake"
  "rclcpp"
  "nav_msgs"
)

foreach(X IN LISTS DEPENDENCIES)
    find_package(${X} REQUIRED)
endforeach()


# find_package(ament_cmake REQUIRED)
# find_package(rclcpp REQUIRED)
# find_package(nav_msgs REQUIRED)



set(EXECUTABLE_NAME "localization_error_calculator_node")


add_executable(${EXECUTABLE_NAME} src/localization_error_calculator.cpp)
ament_target_dependencies(${EXECUTABLE_NAME} ${DEPENDENCIES})

target_include_directories(${EXECUTABLE_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>)
target_compile_features(${EXECUTABLE_NAME} PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17


install(TARGETS ${EXECUTABLE_NAME}
  DESTINATION lib/${PROJECT_NAME})

ament_package()
