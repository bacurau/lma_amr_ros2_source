#!/bin/bash
#===============================================================================
# Description: Runs all the scripts, i.e., installs dependencies, builds
# micro-ROS and runs the AMR ros2 project.
# Author: Ivan Diniz Dobbin
#===============================================================================
# -u breaks the script because of COLCON_CURRENT_PREFIX: unbound variable.
# But, even with these everythings seems normals. Could not find an issue.
# https://www.gnu.org/software/bash/manual/html_node/The-Set-Builtin.html
set -euo pipefail
export ROS_DOMAIN_ID=25
scripts_path="./scripts"
cd ..
$scripts_path/install_dependencies.sh
colcon build
$scripts_path/create_and_build_microROS_agent.sh
$scripts_path/run_project.sh