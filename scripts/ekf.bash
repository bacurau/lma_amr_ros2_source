#!/bin/bash
cd ..
colcon build --packages-select robot_localization
source install/setup.bash
ros2 launch robot_localization ekf.launch.py
# session_name="ekf_test"
# window_name="window1"
# full_path="$session_name:$window_name"
# tmux new -s $session_name -d
# tmux rename-window -t $session_name:0 $window_name
# tmux split-window -h
# tmux send-keys -t $full_path.0 "ros2 launch robot_localization ekf.launch.py" Enter  
# tmux send-keys -t $full_path.1 "ros2 bag play ../bags/current_bag2 -r 5.0; tmux kill-session" Enter                  # send 2nd command 'htop -t' to 2nd pane. I believe there's a `--target` option to target specific pane.
# tmux a;    


