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

handler(){
    echo "Exiting"
    exit 0;
}

export ROS_DOMAIN_ID=25
build_type="${1:-Release}"
scripts_path=$(dirname "${BASH_SOURCE[0]}") # Path to the scripts folder
cd "$scripts_path/.." # navigate to root of ros2 project. It is the directory where you can see src,build,install,log in a ros2 project.

. ./scripts/menu.bash # imports menu function

#================================================================================================================
#=====================  Build and run or just run ros2 project =================================================
#================================================================================================================
create_menu "Build And Run" "Just Run"

if [[ $choosen_item -eq 0 ]]; then
    # ========== Install the dependencies and build ros2 packages properly. ======
    sudo apt update && rosdep update && rosdep install --from-paths src --ignore-src -y 
    # # ========== Build ROS2 packages ===============
    colcon build  --base-path "./src" --symlink-install  --cmake-args "-DCMAKE_BUILD_TYPE=$build_type"
    # ##===================== Creates and Builds micro-ROS Agent =====================##
    # # The instructions were base on
    # # https://micro.ros.org/docs/tutorials/core/first_application_linux/.
    # # However, we just needed to create and build the agent, the firmware is not needed here.
    # # The micro-ROS agent is run in the launch file amr_launch.py
    source ./install/local_setup.bash &&
        ros2 run micro_ros_setup create_agent_ws.sh &&             
            ros2 run micro_ros_setup build_agent.sh
fi

#===========================================================================
##===================== Launch the amr_launch file =========================
#============================================================================

. ./install/local_setup.bash
create_menu "Run real robot" "Run simulation"
[[ $choosen_item -eq 0 ]] && launch_file=amr_launch.py || launch_file=amr_launch_simulation.py
create_menu "I am using a ros2 bag." "I am not using ros2 bags."
[[ $choosen_item -eq 0 ]] && run_launch_with_bags=true || run_launch_with_bags=false

echo $run_launch_with_bags
echo $launch_file
ros2 launch amr_launch $launch_file run_launch_with_bags:=$run_launch_with_bags 



