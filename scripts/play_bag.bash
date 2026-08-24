#!/bin/bash

. ./menu.bash

session_name="bag_playing"
scripts_path=$(dirname "${BASH_SOURCE[0]}") # Path to the scripts folder
cd "$scripts_path"

mapfile -t list_of_bags_with_path < <(ls -d /persistent/items_not_related_to_main_code/process_data/bags/*/) 

mapfile -t list_of_bags_without_path < <(basename -a ${list_of_bags_with_path[@]})
create_menu "${list_of_bags_without_path[@]}"

bag_name="${list_of_bags_with_path[$choosen_item]}"
topics_to_exclude=("/odom" "/odometry/filtered" "/odom_from_imu" "/new_imu_data" "/tf" "/tf_static")

tmux new-session -d -s $session_name

tmux split-window -h

tmux send-keys -t $session_name:0.1 "./main.bash ; sleep 5; exit 0" ENTER

tmux send-keys -t $session_name:0.0 "export ROS_DOMAIN_ID=25  && ros2 bag play $bag_name --exclude-topics ${topics_to_exclude[*]} -p; sleep 5; exit 0" ENTER

tmux attach -t $session_name

