#!/bin/bash

session_name="bag_recording"
bag_name="/persistent/items_not_related_to_main_code/process_data/bags/5.298m_straight_line"
topics_to_exclude=("/odom" "/odometry/filtered" "/odom_from_imu" "/new_imu_data" "/tf" "/tf_static")

tmux new-session -d -s $session_name

tmux split-window -h

tmux send-keys -t $session_name:0.0 "./main.bash ; exit 0" ENTER
tmux send-keys -t $session_name:0.1 "export ROS_DOMAIN_ID=25  && ros2 bag play $bag_name --exclude-topics ${topics_to_exclude[*]} -p; sleep 5; exit 0" ENTER

tmux attach -t $session_name

