#!/bin/bash

# Description:  Creates a menu
#
# Args: 1- the index of the MENU_MESSAGE to be shown in the modal. All the messages can be checked in the debug_variables.bash file.
#       2- List of options to be shown in the menu.
create_menu(){
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
