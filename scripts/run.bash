#!/bin/bash
## This script closes the vscode terminal if it is executed with source ./file.sh or . ./file.sh.
# It might no close on the first execution, but it can close on later executions.
set -eo pipefail

source /opt/ros/jazzy/local_setup.bash
export GZ_VERSION="harmonic" # This is required for detection of which version of gazebo will be used.

# eval "$(register-python-argcomplete ros2)"
# eval "$(register-python-argcomplete colcon)"

##===================== Build ros2 project =======================##
colcon build --base-path "./src" --symlink-install --cmake-args "-DCMAKE_BUILD_TYPE=$build_type" --packages-select lidar_cal

source ./install/local_setup.bash

ros2 run lidar_cal lidar_cal