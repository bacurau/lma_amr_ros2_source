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
trap handler SIGINT

#================================================================================================================
#===================== Var and function definition =================================================
#================================================================================================================

MAGENTA='\033[0;35m'
NC='\033[0m' # No Color
# Capture ctrl + c (SIGINT) and exists the code properly
handler(){
    echo -e "${MAGENTA}Exiting${NC}"
    exit 0;
}
launch_file=("amr_launch.py" "amr_launch_simulation.py")
build_types=("Release" "Debug")
export ROS_DOMAIN_ID=25
YES=0
NO=1

scripts_path=$(dirname "${BASH_SOURCE[0]}") # Path to the scripts folder
cd "$scripts_path/.." # navigate to root of ros2 project. It is the directory where you can see src,build,install,log in a ros2 project.

. ./scripts/menu.bash # imports menu function
install_dependencies_and_build_function(){
    # ========== Install the dependencies and build ros2 packages properly. ======
    sudo apt update && rosdep update && rosdep install --from-paths src --ignore-src -y 
    # # ========== Build ROS2 packages ===============
    colcon build  --base-path "./src" --symlink-install  --cmake-args "-DCMAKE_BUILD_TYPE=$1"
    # ##===================== Creates and Builds micro-ROS Agent =====================##
    # # The instructions were base on
    # # https://micro.ros.org/docs/tutorials/core/first_application_linux/.
    # # However, we just needed to create and build the agent, the firmware is not needed here.
    # # The micro-ROS agent is run in the launch file amr_launch.py
    source ./install/local_setup.bash &&
        ros2 run micro_ros_setup create_agent_ws.sh &&             
            ros2 run micro_ros_setup build_agent.sh
}


#================================================================================================================
#=====================  Build and run or just run ros2 project =================================================
#================================================================================================================


if [[ ! -z "$1" ]]; then # Number used to check that this script was called by play_bag.bash and to choose a launch file. 
    came_from_play_bag_script=$YES
    run_launch_with_bags=true
    choosen_launch_file=${launch_file[$1]}
else
    came_from_play_bag_script=$NO
    run_launch_with_bags=false
fi


# Standard runs the real robot with all the sensors
create_menu "Standard configuration" "Custom configuration"

if [[ $choosen_item -eq 0 ]]; then
    create_menu "Just Run" "Build and Run"
    if [[ $choosen_item -eq 1 ]]; then
        install_dependencies_and_build_function "${build_types[0]}"
    fi
    choosen_launch_file=${launch_file[0]}
else 
    create_menu "Just Run" "Build And Run"
    if [[ $choosen_item -eq 1 ]]; then
        build_types=("Release" "Debug")
        create_menu "${build_types[@]}"
        install_dependencies_and_build_function "${build_types[$choosen_item]}"
    fi
    if [[ $came_from_play_bag_script -eq $NO ]]; then
        create_menu "Run real robot" "Run simulation"
        [[ $choosen_item -eq 0 ]] && using_simulation=$YES || using_simulation=$NO 
        choosen_launch_file=${launch_file[$choosen_item]}
    fi
fi
#===========================================================================
#===================== Launch the amr_launch file =========================
#============================================================================

. ./install/local_setup.bash
# When run_launch_with_bags is true, the launch file will exclude the joystick node for the real robot and simulation
# and the sensors drivers for the real robot.
ros2 launch amr_launch $choosen_launch_file run_launch_with_bags:=$run_launch_with_bags