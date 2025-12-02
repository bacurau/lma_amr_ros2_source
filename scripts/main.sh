#!/bin/bash
#===============================================================================
# Description: installs the dependencies; builds ros2 project; creates and 
# builds micro-ROS agent; change ROS_DOMAIN_ID to 25; and runs this ros2 
# project.
# Author: Ivan Diniz Dobbin
#===============================================================================



## This script closes the vscode terminal if it is executed with source ./file.sh or . ./file.sh. 
# It might no close on the first execution, but it can close on later executions.


set -euo pipefail
scripts_path="./" # Path to the scripts folder
$scripts_path/install_dependencies.sh # Install dependencies
cd ..; colcon build --symlink-install; cd ./scripts  # Build ROS2 packages
$scripts_path/create_and_build_microROS_agent.sh # Create and build micro-ROS agent
$scripts_path/run_project.sh # Run all ros2 packages from the AMR project (lma_amr_ros2_source)