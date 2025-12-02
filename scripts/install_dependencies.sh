#!/bin/bash
#===============================================================================
# Description: Installs the ros2 project dependencies with rosdep.
# Author: Ivan Diniz Dobbin
#===============================================================================

# -u breaks the script because of COLCON_CURRENT_PREFIX: unbound variable.
# But, even with these everythings seems normals. Could not find an issue.
# https://www.gnu.org/software/bash/manual/html_node/The-Set-Builtin.html

set -eo pipefail

##===================== Install dependencies =====================##
cd .. # Navigate to the root of the workspace, so you can install the dependencies and build ros2 packages properly.
sudo apt update && rosdep update
rosdep install --from-paths src --ignore-src -y


