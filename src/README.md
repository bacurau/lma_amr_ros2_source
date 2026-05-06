## Summary

This folder contains the main code for this project.

### Documentation
Every folder and package should have a README.md explaining the folder/package and how it should be used.

### Organization

In the src folder you will have a package or a folder containing packages named ```amr_name```.
A folder is created when multiple packages address a similar subject. For example: amr_sensors has all the packages related to sensors.
Every package should have their own launch file, with the exception of submodules, and every folder should have a main launch file for all the packages
contained in that folder. For example: imagine you have a folder called amr_flying, and 2 packages inside (turbine, wings). You should have 3 launch files, one for the turbine, one for the wings and one main launch to launch both packages together.

Every package will have one or more of the following folders.
- include: header files (.hpp).
- launch: launch files.
- param: parameter files used by the launch files. A parameter file is usually an yaml file with configurations, such as frame_id, ip, etc.
- src: source files (.cpp).


#### Folders(f)/packages(p) descriptions

- src/amr_launch (p): launches the ros2 project by launching the other packages in the right order with the correct configuration.
- src/amr_description (p): contains urdf and other files about the robot model.
- src/amr_localization (f): contains a package for odometry and a launch package to launch all packages in this folder. Currently, the only package (besides the launch one) is odometry. Other packages will probably be implemented in the future.
- src/amr_sensors (f): contains a launch package to launch all packages in this folder, and 3 ros2 sensor drivers packages: velodyne (3D-lidar), sick(2D-lidar) and IMU.
- src/amr_stm32 (f): contains the micro-ROS package and a package for resetting the stm32 board so a proper connection can be made via micro-ROS.


### Run the project

1. Navigate to the scripts folder:

```
cd  /persistent/scripts
```

2. Execute the main.bash script.

```
./main.bash
```


> [!IMPORTANT]
> To build and run the project, use the scripts. The setup to use micro-ROS is only made on the scripts.
> If have not built with the scripts and launch directly using ```ros2 launch amr_launch amr_launch.py```, the micro-ROS communication will not work properly.
> If you have already build with the scripts, ```ros2 launch amr_launch amr_launch.py``` will work. 







