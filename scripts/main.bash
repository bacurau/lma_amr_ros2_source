#!/bin/bash
#===============================================================================
# Description: installs the dependencies; builds ros2 project; creates and
# builds micro-ROS agent; change ROS_DOMAIN_ID to 25; and runs this ros2
# project.
# Author: Ivan Diniz Dobbin
#===============================================================================



## This script closes the vscode terminal if it is executed with source ./file.sh or . ./file.sh.
# It might no close on the first execution, but it can close on later executions.
set -eo pipefail
export ROS_DOMAIN_ID=25
YES=0
NO=1
RUN_IN_SIMULATION="${1:-$NO}"
RUN_PROJECT="${2:-$YES}"
build_type="${3:-Release}"
scripts_path="./scripts" # Path to the scripts folder

source /opt/ros/jazzy/local_setup.bash



# cd .. # Navigate to the root of the workspace, so you can install the dependencies and build ros2 packages properly.
#$scripts_path/install_dependencies.bash # Install dependencies
##===================== Install dependencies =====================##
sudo apt update && rosdep update
rosdep install --from-paths src --ignore-src -y



##===================== Build ros2 project =======================##
colcon build  --base-path "./src" --symlink-install  --cmake-args "-DCMAKE_BUILD_TYPE=$build_type"
#cd ./scripts  # Build ROS2 packages

##===================== Creates and Builds micro-ROS Agent =====================##
# The instructions were base on
# https://micro.ros.org/docs/tutorials/core/first_application_linux/.
# However, we just needed to create and build the agent, the firmware is not needed here.
# The micro-ROS agent is run in the launch file amr_launch.py
source ./install/local_setup.bash &&
    ros2 run micro_ros_setup create_agent_ws.sh &&
        ros2 run micro_ros_setup build_agent.sh


exit

if [[ "$RUN_PROJECT" =  "$YES" ]]; then
    cd $scripts_path; ./run_project.bash $RUN_IN_SIMULATION # Run all ros2 packages from the AMR project (lma_amr_ros2_source)
fi