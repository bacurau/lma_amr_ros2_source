#!/bin/bash

session_name="bag_recording"
bag_name="long_path_l_r_0.102986_sep_0.742188"
tmux new-session -d -s $session_name
topics_to_exclude=("/odom" "/odometry/filtered" "/odom_from_imu" "/new_imu_data" "/tf" "/tf_static" "/corrected_angular_velocities_from_imu")
tmux split-window -h

tmux send-keys -t $session_name:0.0 "./main.bash ; sleep 5; exit 0" ENTER
tmux send-keys -t $session_name:0.1 "export ROS_DOMAIN_ID=25 && sleep 30; cd ../bags && ros2 bag record --all-topics --exclude-topics ${topics_to_exclude[*]} -o $bag_name ; sleep 5;exit 0" ENTER

tmux attach -t $session_name

