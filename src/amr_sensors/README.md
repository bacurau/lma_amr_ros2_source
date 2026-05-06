## Summary
This folder is responsible for all packages related to sensors. Additional information for the configuration of the sensors can be found at the wiki:
- https://github.com/bacurau/lma_amr_ros2_source/wiki/AMR-Sensors-and-ROS-2-Driver-Integration
- https://github.com/bacurau/lma_amr_ros2_source/wiki/Setting-up-the-velodynes


Current packages:
- Not submodule:
    - amr_sensors_launch: is responsible for launching all of the submodules with their correct parameter files.
- Submodules:
    - sick_scan_xd: get data from sick (2d lidar ) and converts to ros2 topics.
    - v4l2_camera: gets data from camera and converts to ros2 topics.
    - velodyne (is a folder containing more packages): gets data from velodyne (3d lidar) and coverts to ros2 topics.
    - Xsens_MTI_ROS_Driver_and_Ntrip_Client: gets data from imu MTI-680g and converts to ros2 topics.


### Detached Use
To use only the packages contained in this folder, follow the next instructions.
To use it with all the other packages, check the README.md of amr_launch.


1. Navigate to the root of your project
2. Build the project.

```
colcon build --paths src/amr_sensors/*
```

3. Source your project. 

```
. ./install/local_setup.bash
```

4. Run the launch file.

```
ros2 launch amr_sensors_launch sensors_main_launch.py
```