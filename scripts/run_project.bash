#!/bin/bash
#===============================================================================
# Description: Runs amr ros2 project
# Author: Ivan Diniz Dobbin
#===============================================================================

# -u breaks the script because of COLCON_CURRENT_PREFIX: unbound variable.
# But, even with these everythings seems normals. Could not find an issue.
# https://www.gnu.org/software/bash/manual/html_node/The-Set-Builtin.html
set -eo pipefail

##===================== Launch the amr_launch file =====================##
cd .. # Navigate to the root of the workspace.
RUN_IN_SIMULATION=$1
YES=0
NO=1
export ROS_DOMAIN_ID=25 # Use the same ROS_DOMAIN_ID as the one used in the STM32 board (micro-ROS).

. ./install/local_setup.bash &&  [[ "$RUN_IN_SIMULATION" = "$YES" ]]  && ros2 launch amr_launch amr_launch_simulation.py || ros2 launch amr_launch amr_launch.py
