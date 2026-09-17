#!/bin/bash
#===================================================================================
# Description: starts the ros2 project and plays a ros2 bag of your choosing.
# The ros2 launch file identifies the use of a ros2 bag through the 
# variable run_launch_with_bag. This variable value is set with the bash variable 
# RUN_LAUNCH_WITH_BAG. 
# Author: Ivan Diniz Dobbin
#===================================================================================

set -eo pipefail

#================================================================================================================
#===================== Define variables =================================================
#================================================================================================================
scripts_path=$(dirname "${BASH_SOURCE[0]}") # Path to the scripts folder
cd "$scripts_path" # navigate to scripts folder.
. ./config.bash # imports menu function

export RUN_LAUNCH_WITH_BAG=true
session_name="bag_playing"

#================================================================================================================
#===================== Define functions =================================================
#================================================================================================================
# Description:  Creates a menu
#
# Args: 1- the index of the MENU_MESSAGE to be shown in the modal. All the messages can be checked in the debug_variables.bash file.
#       2- List of options to be shown in the menu.
Create_Menu(){
    # get array and option from the arguments passed
    local menu_list=()
    #local which_menu="$1"    # take the first argument
    #shift               # remove the first argument from the list of arguments
    local list=("$@") # take the rest of the arguments. In this case, the list of options to be used in the menu.
    # create a list in the format that menu expects that is [id] [name], example "1 my_package"
    for i in "${!list[@]}"; do
        menu_list+=( "$((i+1))" "${list[$i]}" )
    done   
    # create menu
    # menu outputs to descriptor 2 (which is stderr normally), so we create a third descriptor and change things
    # 0: stdin, 1: stdout, 2: stderr
    # 1- Create a third descriptor and make it point to stdout. 1: stdout, 2: stderr ,3:stdout
    # 2- Now make the descriptor 1 (stdout) point to descriptor 2 (stderr).1: stderr, 2: stderr ,3:stdout 
    # 3- Finally point the descriptor 2 (stderr) to descriptor 3(now stdout). 1: stderr, 2: stdout ,3:stdout
    
    item=$(dialog --keep-tite --title "Choose an option" \
       --menu "" 0 0 0 "${menu_list[@]}" 3>&1 1>&2 2>&3)
    status=$?
    # if cancel button is pressed, terminate the program.
    [[ $status -eq 1 ]] && exit 0
    choosen_item=$((item - 1))
}

#================================================================================================================
#===================== Body =====================================================================================
#================================================================================================================
# ====== Creates a menu list with all the bags in the path you passed to BAGS_PATH ===============
mapfile -t list_of_bags_with_path < <(ls -d "$BAGS_PATH"/*/) 
mapfile -t list_of_bags_without_path < <(basename -a ${list_of_bags_with_path[@]})
Create_Menu "${list_of_bags_without_path[@]}"
bag_name="${list_of_bags_with_path[$choosen_item]}"

# =========== Creates a command to exclude or choose some topic from the bag ============================
# flag to exclude topics, followed by the topics, and then add a clock with 50 hz
topics_string_command=("--exclude-topics" "/odom" "/odometry/filtered" "/odom_from_imu" "/new_imu_data" "/tf" "/tf_static" "/clock" "--clock" "100")
if [[ "$USE_SIMULATION" = true ]]; then
  # flag to choose topics, followed by choosen topics.
  topics_string_command=("--topics" "/cmd_vel")
fi

# ========== Plays bag in a tmux session ==================
# === Creates a tmux session and splits screen in two terminal, one for playing the bag and the other for executing the ros2 project 
tmux new-session -d -s $session_name
tmux split-window -h
tmux send-keys -t $session_name:0.1 "./main.bash; sleep 5; exit 0" ENTER
tmux send-keys -t $session_name:0.0 "ros2 bag play $bag_name ${topics_string_command[*]} -p; sleep 5; exit 0" ENTER
tmux attach -t $session_name

