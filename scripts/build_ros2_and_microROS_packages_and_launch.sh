#!/bin/bash
#===============================================================================
# File: build_ros2_and_microROS_packages
# Description: Builds ROS2 and micro-ROS packages for the AMR project and then runs
# the launch file for the amr project.
# Author: Ivan Diniz Dobbin
#===============================================================================


## This file should only setup rosdep and microROS.

## This script closes the vscode terminal if it is executed with source ./file.sh or . ./file.sh 
# because of the command bash. It might no close on the first execution, but it can close on later executions.

set -euo pipefail
# set -x

# cd ../../../ # Navigate to the root of the workspace

cd ../
export ROS_DOMAIN_ID=25
colcon build


## According to the ros2 documentation, it is necessary to open a new 
# terminal or in this case a new bash shell to source the workspace
# ". ../install/local_setup.bash". 

##===================== Creates and Builds micro-ROS Agent =====================##
# The instructions were base on
# https://micro.ros.org/docs/tutorials/core/first_application_linux/.
# However, we just needed to create and build the agent, the firmware is not needed here.
# The micro-ROS agent is run in the launch file amr_launch.py

bash -c "   . ./install/local_setup.bash &&
            ros2 run micro_ros_setup create_agent_ws.sh &&             
            ros2 run micro_ros_setup build_agent.sh
        "

## TODO: Remove launch from here, this file should only setup rosdep and microROS.
##===================== Launch the amr_launch file =====================##
bash -c "   . ./install/local_setup.bash &&  
            ros2 launch amr_launch amr_launch.py
"
 #           ros2 launch amr_launch amr_launch.py
 #       "
