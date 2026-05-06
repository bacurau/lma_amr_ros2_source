## Summary

Package responsible for calculating odometry for a differential driven robot.

## Independent use
To use this package alone, follow the next instructions.
To use it with all the other packages, check the README.md of amr_launch.

1. Navigate to the root of your project
2. Build the project.

```
colcon build --packages-select odometry
```

3. Source your project. 

```
. ./install/local_setup.bash
```

4. Run the launch file.

```
ros2 launch odometry odometry_launch.py
```

5. To calculate odometry, publish a message to the topic joint_states (sensor_msgs/msg/JointState) in another terminal. To see the format of the message: ```ros2 interface show sensor_msgs/msg/JointState```. 

```
ros2 topic pub /joint_states sensor_msgs/msg/JointState "
header:
  stamp:
    sec: 0
    nanosec: 0
  frame_id: ''
name: ['joint1', 'joint2']
position: [1.57, 0.5]
velocity: [0.0, 0.0]
effort: [0.0, 0.0]
" -r 1
```

This will publish the same message at 1hz. This is just so you can see the callbacks being called, but as you do not change joint position, the odom will return 0 for the speeds

```
twist:
  twist:
    linear:
      x: 0.0
      y: 0.0
      z: 0.0
    angular:
      x: 0.0
      y: 0.0
      z: 0.0
```

6. Open another terminal and check the /odom topic.

```
ros2 topic echo /odom
```