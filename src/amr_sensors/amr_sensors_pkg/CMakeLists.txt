#Set minimum required CMake version and project name.
cmake_minimum_required(VERSION 3.5)
project(amr_sensors)


# Set C++ standard, default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(xsens_mti_ros2_driver REQUIRED)

# Check ROS distro
if(DEFINED ENV{ROS_DISTRO})
  set(ROS_DISTRO $ENV{ROS_DISTRO})
else()
  set(ROS_DISTRO "unknown")
endif()

message(STATUS "Detected ROS_DISTRO: ${ROS_DISTRO}")

###########
## Build ##
###########

#############
## Install ##
#############

# all install targets should use catkin DESTINATION variables
# See http://ros.org/doc/api/catkin/html/adv_user_guide/variables.html

install(DIRECTORY launch param 
  DESTINATION share/${PROJECT_NAME}
)

install(DIRECTORY headers/
  DESTINATION include/${PROJECT_NAME}
  FILES_MATCHING PATTERN "*.h"
)


ament_export_include_directories(
	include
)

ament_package()
