## Summary 
This package is responsible for launching every sensor submodule package.

launch:
- imu.launch.py: launches imu with the parameter file xsens_params.yaml.
- lidar.launch.py: lidar sick main launch file. Runs 1 secondary launch file for every sick lidar (2 in total):sick_back.launch and sick_front.launch.
- velodyne_launch.py: velodyne main launch file. Runs a secondary launch file for both velodynes: velodyne_VLP16_individual_launch.py.
- sensors_main_launch.py: Launches the sensors main launch files: imu.launch.py, lidar.launch.py, and velodyne_launch.py. 
- sick_back.launch: launch the sick lidar, present in the back of the robot,  with the necessary configurations. The original file can be found in the sick_scan_xd package ```sick_scan_xd/launch/sick_tim_5xx.launch```.
- sick_front.launch: launch the sick lidar, present in the front of the robot,  with the necessary configurations. The original file can be found in the sick_scan_xd package ```sick_scan_xd/launch/sick_tim_5xx.launch```.
- velodyne_VLP16_individual_launch.py: this file receives launch arguments and parameters to launch the chosen velodyne. The original file did not receive launch arguments, but it can be found int the velodyne package ```velodyne/velodyne/launch/velodyne-all-nodes-VLP16-launch.py```.

param:
- velodyne_back_left_params.yaml: parameter file for the velodyne positioned in the back left corner.
- velodyne_front_right_params.yaml: parameter file for the velodyne positioned in the front right corner.
> [!IMPORTANT]
> Most important parameters for the velodyne are: device_ip, frame_id, port
- xsens_params.yaml: parameter file for the imu.
> [!IMPORTANT] 
> Any configuration will only take place if ```enable_deviceConfig: true ```.

### Detached Use
Check the README.md of amr_sensors.

