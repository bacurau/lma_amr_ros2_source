#!/bin/bash

cd ..
if [[ "$1" -eq 0 ]]; then
  colcon build --packages-select marvelmind_ros2_msgs marvelmind_ros2 plot_marvelmind
fi

. ./install/local_setup.bash
pwd
mkdir -p bags
session_name="experiment"
ros2 launch plot_marvelmind marvelmind_ros2.launch.py
# ========== Plays bag in a tmux session ==================
# === Creates a tmux session and splits screen in two terminal, one for recording the bag and the other for executing the ros2 project 
# =========== Recording the bag in a tmux session ====================
#tmux new-session -d -s $session_name
#tmux split-window -h
#tmux send-keys -t $session_name:0.0 "ros2 launch plot_marvelmind marvelmind_ros2.launch.py ; sleep 5; exit 0" ENTER
#tmux send-keys -t $session_name:0.1 "ros2 bag record --all-topics -o ./bags/marvelmind_precision_experiment  --start-paused; sleep 5;exit 0" ENTER
#tmux attach -t $session_name




