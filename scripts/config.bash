#!/bin/bash

#===================================================================================
# Description: configuration file. This file is used to determine if you are going
# to build, run, use simulation and other options for your ros2 project.
# Author: Ivan Diniz Dobbin
#===================================================================================

trap Handler SIGINT
export ROS_DOMAIN_ID=25
export BUILD_PROJECT=false # true: builds ros2 project | false: does not build ros2 project
export RUN_PROJECT=true # true: runs ros2 project with a launch file | false: does not run the ros2 project
export USE_SIMULATION=true # true: runs project with a gazebo simulation | false: runs project with the real robot.
export BUILD_MICROROS=false # true: build and configurate micro-ROS for communication with stm32board| false: will not build and configurate the micro-ROS,
# however, if this was already done before, the micro-ROS communication will probably work after the ros2 launch.
export BAGS_PATH="/persistent/items_not_related_to_main_code/process_data/bags" # pass the full path of the location of your bags. This will be used to play and record bags to this place.

[[ "$USE_SIMULATION" = true ]] && export LAUNCH_FILE="amr_launch_simulation.py" || export LAUNCH_FILE="amr_launch.py" 
# true: use the ros2 launch file for simulations | false: use the ros2 launch file for the real robot

# defining colors
MAGENTA='\033[0;35m'
NC='\033[0m' # No Color

# Capture ctrl + c (SIGINT) and exists the code properly
Handler(){
    echo -e "${MAGENTA}Exiting${NC}"
    exit 0;
}


