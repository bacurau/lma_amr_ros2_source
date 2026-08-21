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
#================================================================================================================
#===================== Var and function definition =================================================
#================================================================================================================

export ROS_DOMAIN_ID=25
build_type="${1:-Release}"
scripts_path=$(dirname "${BASH_SOURCE[0]}") # Path to the scripts folder
cd "$scripts_path/.." # navigate to root of ros2 project. It is the directory where you can see src,build,install,log in a ros2 project.

# Description:  Creates a menu
#
# Args: 1- the index of the MENU_MESSAGE to be shown in the modal. All the messages can be checked in the debug_variables.bash file.
#       2- List of options to be shown in the menu.
create_menu(){
    # get array and option from the arguments passed
    local menu_list=()
    #local which_menu="$1"    # take the first argument
    #shift               # remove the first argument from the list of arguments
    local list=("$@") # take the rest of the arguments. In this case, the list of options to be used in the menu.
    # create a list in the format that menu expects that is [id] [name], example "1 my_package"
    for i in "${!list[@]}"; do
        menu_list+=( "$((i+1))" "${list[$i]}" )
    done   
    # create menu
    # menu outputs to descriptor 2 (which is stderr normally), so we create a third descriptor and change things
    # 0: stdin, 1: stdout, 2: stderr
    # 1- Create a third descriptor and make it point to stdout. 1: stdout, 2: stderr ,3:stdout
    # 2- Now make the descriptor 1 (stdout) point to descriptor 2 (stderr).1: stderr, 2: stderr ,3:stdout 
    # 3- Finally point the descriptor 2 (stderr) to descriptor 3(now stdout). 1: stderr, 2: stdout ,3:stdout
    
    item=$(dialog --keep-tite --title "Choose an option" \
       --menu "" 0 0 0 "${menu_list[@]}" 3>&1 1>&2 2>&3)
    status=$?
    # if cancel button is pressed, terminate the program.
    [[ $status -eq 1 ]] && exit 0
    choosen_item=$((item - 1))
}




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
if [[ $choosen_item -eq 0 ]]; then
    ros2 launch amr_launch amr_launch.py
else
    ros2 launch amr_launch amr_launch_simulation.py 
fi



