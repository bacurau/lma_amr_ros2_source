## Summary

This repository provides an overview of the code structure for the Autonomous Mobile Robot (AMR) developed in the Laboratory of Autonomous Mobility.


### Main functionalities:
- A simulated version of the robot for gazebo simulation and ros2 tf2 trees;
- A simulated world that can be edited for different scenarios
- Integration of 3 driver sensors to get data for the real robot: velodyne (3D-lidar), sick(2D-lidar) and IMU;
- Odometry calculation for the real robot;
- Communication with the embedded system using micro-ROS;
- Launch files to facilitate the use of individual packages or packages contained inside a folder;   
- Scripts to run all packages and additional configurations of this project.


### Organization

items_not_related_to_main_code: useful items related to the project, such as a code to get all ros2 topics with their publishers and subscribers, and a ros2 graph of the nodes, topics and their connections. However, they are not a part of the main code.
src: contains the main code for this ros2 project.
scripts: contains the scripts necessary to run the project.

### Run the project

1. To run the project in the robot: please check the wiki for instructions.
- https://github.com/bacurau/lma_amr_ros2_source/wiki/Accessing-the-robot : teaches you how to access the robot laptop.
- https://github.com/bacurau/lma_amr_ros2_source/wiki/Setting-up-docker : teaches you how to set up the docker container.
- https://github.com/bacurau/lma_amr_ros2_source/wiki/How-to-run-the-code: teaches you how to run the code.


