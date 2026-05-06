## Summary
This folder is responsible for all packages related to the communication between ROS (laptop) and micro-ROS (STM32 board).

Current packages:
- Not submodule:
    - amr_stm32_launch: is responsible for resetting the stm32 board and launching the micro_ros_agent to start the communication between micro-ROS and ROS.
- Submodules:
    - micro_ros_setup: responsible for setting up the necessary elements needed for the communication between ROS and micro-ROS.

### Detached Use
To create a micro-ROS agent which will allow communication with the micro-ROS, follow the next instructions.
To use it with all the other packages, check the README.md of amr_launch.

1. Navigate to the root of your project
2. Build the project.

```
colcon build --paths src/amr_stm32/*
```

3. Source your project. 

```
. ./install/local_setup.bash
```

4. Run the micro agent scripts.

```
ros2 run micro_ros_setup create_agent_ws.sh && ros2 run micro_ros_setup build_agent.sh
```

5. In another terminal window.

```
. ./install/local_setup.bash
```
6. And then

```
ros2 launch amr_stm32_launch stm32_launch.xml
```


> [!NOTE]
> Different from the other packages, you need to build, run nodes for set up, and then launch the package that you want.
> Another note: the micro_ros_agent node is present inside of the uros folder. A folder which is created during build and node execution.
