#!/bin/bash
## This script closes the vscode terminal if it is executed with source ./file.sh or . ./file.sh.
# It might no close on the first execution, but it can close on later executions.
set -euo pipefail

set +u
source ./install/setup.bash
set -u

ros2 bag info bags/rosbag2_2026_09_07-13_42_27
bag=bags/rosbag2_2026_09_07-13_42_27

bag=bags/rosbag2_2026_09_27-19_05_51
# bag=bags/rosbag2_2026_09_27-20_03_04
# ros2 bag play ${bag} --loop --topics /Sick_Cloud_Back /Sick_Cloud_Front
ros2 bag play ${bag} --loop