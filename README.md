## README

Functionalities:
- created a gazebo model for simulation and ros2 tf2 trees.
- added 3 ros2 driver sensors: velodyne (3D-lidar), sick(2D-lidar) and IMU.
- implemented wheel odometry.
- configurated micro-ROS to communicate with the embedded system.


Folders description:

- src/amr_launch: launches the ros2 project by launching the other packages in the right order with the correct configuration.
- src/amr_description: contains urdf and other files about the robot model.
- src/amr_localization: contains a package for odometry and a launch package to launch all packages in this folder. Currently, the only package (besides the launch one) is odometry. Other packages will probably be implemented in the future.
- src/amr_sensors: contains a launch package to launch all packages in this folder, and 3 ros2 sensor drivers packages: velodyne (3D-lidar), sick(2D-lidar) and IMU.
- src/amr_stm32: contains the micro-ROS package and a package for reseting the stm32 board so a proper connection can be made via micro-ROS.
- scripts: folder which contains all the scripts necessary to launch the ros2 project.
- wiki artifacts: contains wiki artifacts, such as a code to get all ros2 topics with their publishers and subscribers, and a ros2 graph of the nodes, topics and their connections.
- my_rosbags: contains 3 rosbags related to odometry experiments.


:open_file_folder

├── my_rosbags
│   ├── odom_bag
│   ├── odom_bag2
│   └── odom_bag3
├── README.md
├── scripts
│   ├── create_and_build_microROS_agent.bash
│   ├── install_dependencies.bash
│   ├── main.bash
│   ├── README.md
│   └── run_project.bash
├── src
│   ├── amr_description
│   ├── amr_launch
│   ├── amr_localization
│   ├── amr_sensors
│   └── amr_stm32
└── wiki_artifacts
    ├── info_ros2.c
    ├── ros2_graph
    └── topic_information.md




