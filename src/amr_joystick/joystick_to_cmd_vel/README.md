## Summary

This package is responsible for receiving the commands given by a controller (ps3 controller, xbox controller, etc), transforming it, and then publishing it to cmd_vel to control a simulated or real robot. Usually, the ros2 topic type is:geometry_msgs/msg/Twist.

Commands from the perspective of a ps3 controller:
- Triangle: goes forward.
- X: goes backward.
- Square: turns left.
- Circle: turns right.
- L1: decreases robot speed.
- R1: increases robot speed.
- Start: start ou pause sending commands via cmd_vel.


> [!IMPORTANT]
> Every controller has different mappings, so, the array of buttons and axis might be different for each controller.
> If you plug a different controller, check the joy topic to see the position of each button in the array.


### Detach use
To use this package alone, follow the next instructions.
To use it with all the other packages, check the README.md of amr_launch.


1. Navigate to the root of your project
2. Build the project.

```
colcon build colcon build --paths src/amr_joystick/*
```

3. Source your project. 

```
. ./install/local_setup.bash
```

4. Run the launch file.

```
ros2 launch joystick_to_cmd_vel joystick_to_cmd_vel_launch.py
```

5. To verify that commands are being sent to cmd_vel, open a new terminal and echo the cmd_topic.

```
ros2 topic echo /cmd_vel
```

6. Press start to starting sending messages to cmd_vel, and then press the appropriate buttons on your controller.