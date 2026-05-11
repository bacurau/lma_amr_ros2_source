#!/usr/bin/env bash
set -xeuo pipefail

SESSION_NAME="gazebo_session"

pkill -f -9 "gz sim" || true # Kill any process of gazebo
tmux kill-session -t "${SESSION_NAME}" 2> /dev/null || true # Kill possible old sessions

set +xeuo pipefail
source /opt/ros/${ROS_DISTRO}/setup.bash
set -xeuo pipefail

export GZ_VERSION="harmonic" # This is requrired for detection of which version of gazebo will be used.
colcon build --cmake-args -DBUILD_TESTING=ON

set +xeuo pipefail
source ./install/setup.bash
set -xeuo pipefail


# Start a new detached session with a dummy command
tmux new-session -d -s "${SESSION_NAME}" -n "dummy" "/usr/bin/env sh -i"
tmux move-window -s "${SESSION_NAME}:1" -t "${SESSION_NAME}:99" # Move it to a less "annoying" index.

COMMANDS=(
    "ros2 launch gz_sim_bringup diff_drive.launch.py"
    # "rqt"
)

# Iterate over the array via its indexes.
for i in "${!COMMANDS[@]}"; do
  cmd="${COMMANDS[$i]}"
  echo Starting: \"${cmd}\"
  # Start a new tmux window, with the command being called via the current
  # shell. Also add a call to an interactive shell at the end if the initial
  # commands errors or terminates, this allows for the user to see what
  # happened.
  tmux new-window -t "${SESSION_NAME}:${i}" "/usr/bin/env ${SHELL} -c \"${cmd}\"; /usr/bin/env ${SHELL} -i"
done

# Attach to the session
tmux attach -t "${SESSION_NAME}"
