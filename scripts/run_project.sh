#!/bin/bash
#===============================================================================
# Description: Runs amr ros2 project
# Author: Ivan Diniz Dobbin
#===============================================================================

set -eo pipefail
##===================== Launch the amr_launch file =====================##
. ./install/local_setup.bash &&  
    ros2 launch amr_launch amr_launch.py
