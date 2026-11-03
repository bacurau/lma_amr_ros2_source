## amr_slam

2D mapping with [slam_toolbox](https://github.com/SteveMacenski/slam_toolbox) (online async mode).

The launch is the same for simulation and for the real robot. It expects `odom_frame -> Base_Link`
from the `odometry` node and a `sensor_msgs/LaserScan`.

### Run in simulation

Terminal 1 (builds and starts Gazebo with `USE_SIMULATION=true` in `scripts/config.bash`):

```bash
cd /persistent/scripts && ./main.bash
```

Terminal 2:

```bash
source /persistent/install/local_setup.bash
ros2 launch amr_slam slam_launch.py
```

Drive the robot (`/cmd_vel`) and watch `/map` in RViz (Fixed Frame `map`).

### Real robot

```bash
ros2 launch amr_slam slam_launch.py use_sim_time:=false scan_topic:=/sick_tim_5xx_front/scan
```

Each real SICK TiM5xx covers 270 deg; the simulated front lidar covers 360 deg. For equivalent
coverage on the real robot the front and back scans need to be merged into one scan first.

### Save the map

```bash
ros2 run nav2_map_server map_saver_cli -f ~/my_map   # needs ros-jazzy-nav2-map-server
```
or serialize the pose graph with the slam_toolbox RViz panel.

### Parameters

`param/slam_toolbox.yaml`. Frames: `odom_frame`, `map`, `Base_Link`.
