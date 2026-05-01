This folder contains all sensors packages.


Not submodule:
- amr_sensors: is responsible for launching all of the submodules with their correct parameter files.

Submodules:
- sick_scan_xd: get data from sick (2d lidar ) and converts to ros2 topics.
- v4l2_camera: gets data from camera and converts to ros2 topics.
- velodyne: gets data from velodyne (3d lidar) and coverts to ros2 topics.
- Xsens_MTI_ROS_Driver_and_Ntrip_Client: gets data from imu MTI-680g and converts to ros2 topics.
