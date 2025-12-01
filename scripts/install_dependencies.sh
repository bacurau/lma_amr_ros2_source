#!/bin/bash
#===============================================================================
# Description: Installs dependencies for ros2 project
# Author: Ivan Diniz Dobbin
#===============================================================================
set -eo pipefail

##===================== Install dependencies =====================##
     sudo apt update && rosdep update
     rosdep install --from-paths src --ignore-src -y


