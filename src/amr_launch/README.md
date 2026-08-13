## Summary

This package is responsible for launching all the main launch files from the other folder and independent nodes as needed, such as robot_state_publisher_node.
Current launch files being integrated:
- amr_description/launch/gazebo_launch
- amr_joystick/joystick_to_cmd_vel/launch/joystick_to_cmd_vel_launch.py
- amr_localization/amr_localization_launch/launch/localization_main_launch.py
- amr_sensors/amr_sensors_launch/launch/sensors_main_launch.py
- amr_stm32/amr_stm32_launch/launch/stm32_launch.xml

> [!IMPORTANT]
> If you have not built the project with the scripts, the micro-ROS communication will fail.
> Do not use the launch directly to launch the project. Use the scripts.

## Use

1. Navigate to the root of your project
2. Build the project.

```
colcon build
```

3. Source your project. 

```
. ./install/local_setup.bash
```

4. Run the launch file.

```
ros2 launch amr_launch amr_launch.py
```

5. The project should be running, but micro-ROS communication will not be working.