#!/bin/bash
#===============================================================================
# Description: Creates and builds the micro-ROS agent for the AMR project
# Author: Ivan Diniz Dobbin
#===============================================================================

# -u breaks the script because of COLCON_CURRENT_PREFIX: unbound variable.
# But, even with these everythings seems normals. Could not find an issue.
# https://www.gnu.org/software/bash/manual/html_node/The-Set-Builtin.html
set -eo pipefail

##===================== Creates and Builds micro-ROS Agent =====================##
# The instructions were base on
# https://micro.ros.org/docs/tutorials/core/first_application_linux/.
# However, we just needed to create and build the agent, the firmware is not needed here.
# The micro-ROS agent is run in the launch file amr_launch.py

source ./install/local_setup.bash &&
    ros2 run micro_ros_setup create_agent_ws.sh &&             
        ros2 run micro_ros_setup build_agent.sh
        
