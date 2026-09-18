#!/bin/bash
#===============================================================================
# Description: installs the dependencies; builds ros2 project; creates and 
# builds micro-ROS agent; change ROS_DOMAIN_ID to 25; and runs this ros2 
# project. Change options in the config.bash file.
# Author: Ivan Diniz Dobbin
#===============================================================================

## This script closes the vscode terminal if it is executed with source ./file.sh or . ./file.sh. 
# It might no close on the first execution, but it can close on later executions.


set -eo pipefail
scripts_path=$(dirname "${BASH_SOURCE[0]}") # Path to the scripts folder
cd "$scripts_path/.." # navigate to root of ros2 project. It is the directory where you can see src,build,install,log in a ros2 project.
. ./scripts/config.bash # imports menu function

###===================== Creates and Builds micro-ROS Agent =====================##
    # # The instructions were base on
    # # https://micro.ros.org/docs/tutorials/core/first_application_linux/.
    # # However, we just needed to create and build the agent, the firmware is not needed here.
    # # The micro-ROS agent is run in the launch file amr_launch.py
Build_Microros(){
    source ./install/local_setup.bash &&
        ros2 run micro_ros_setup create_agent_ws.sh &&             
            ros2 run micro_ros_setup build_agent.sh
}

#===========================================================================
#===================== Build project and micro ros files =========================
#============================================================================
if [[ "$BUILD_PROJECT" = true ]]; then
  # ========== Install the dependencies and build ros2 packages properly. ======
  sudo apt update && rosdep update && rosdep install --from-paths src --ignore-src -y 
  # # ========== Build ROS2 packages ===============
  colcon build  --base-path "./src" --symlink-install  --cmake-args "-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
  [[ "$BUILD_MICROROS" = true ]] && Build_Microros
fi

#===========================================================================
#===================== Launch the amr_launch file =========================
#============================================================================
if [[ "$RUN_PROJECT" = true ]]; then
  . ./install/local_setup.bash
  : ${RUN_LAUNCH_WITH_BAG:=false} # if null or unset, set run_launch_with_bag false. https://www.gnu.org/software/bash/manual/html_node/Shell-Parameter-Expansion.html
  ros2 launch amr_launch $LAUNCH_FILE run_launch_with_bag:=$RUN_LAUNCH_WITH_BAG 
fi




