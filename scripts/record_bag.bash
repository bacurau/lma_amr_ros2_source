#!/bin/bash

#===================================================================================
# Description: starts the ros2 project and records a ros2 bag.
# Author: Ivan Diniz Dobbin
#===================================================================================


set -euo pipefail

#===================== Define variables =================================================
scripts_path=$(dirname "${BASH_SOURCE[0]}") # Path to the scripts folder
cd "$scripts_path" # navigate to scripts folder.
. ./config.bash # imports config file
session_name="bag_recording"
topics_to_exclude=("/odom" "/odometry/filtered" "/odom_from_imu" "/new_imu_data" "/tf" "/tf_static" "/corrected_angular_velocities_from_imu")

# =========== Recording the bag in a tmux session ====================
read -p "Type the name of your bag and press enter:" bag_name
tmux new-session -d -s $session_name
tmux split-window -h
tmux send-keys -t $session_name:0.0 "./main.bash ; sleep 5; exit 0" ENTER
tmux send-keys -t $session_name:0.1 "cd $BAGS_PATH && ros2 bag record --all-topics --exclude-topics ${topics_to_exclude[*]} -o $bag_name  --start-paused; sleep 5;exit 0" ENTER
tmux attach -t $session_name

