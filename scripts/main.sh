#!/bin/bash
#===============================================================================
# Description: installs the dependencies; builds ros2 project; creates and 
# builds micro-ROS agent; change ROS_DOMAIN_ID to 25; and runs this ros2 
# project.
# Author: Ivan Diniz Dobbin
#===============================================================================

set -euo pipefail
scripts_path="./scripts" # Path to the scripts folder
cd .. # Navigate to the root of the workspace, so you can install the dependencies and build ros2 packages properly.
$scripts_path/install_dependencies.sh # Install dependencies
colcon build # Build ROS2 packages
$scripts_path/create_and_build_microROS_agent.sh # Create and build micro-ROS agent
$scripts_path/run_project.sh # Run all ros2 packages from the AMR project (lma_amr_ros2_source)