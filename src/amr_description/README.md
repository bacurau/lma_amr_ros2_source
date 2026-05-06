## Summary

This folder/package is responsible for containing items related to the robot simulation and urdf files used by the node ```robot_state_publisher```
to create a tf2 tree.

Structure:
- launch: responsible for launching the gazebo simulation with the main.sdf file.
- models: contains all the models used by the simulation. A model is usually a single item, such as a robot, a chair, a table, a wall, etc.
    - amr: actual robot model
    - vehicle_blue: simplest model of a differential driven robot with 2 rear wheels and a caster wheel in the front.
    - wall: model for a wall.
    - x1: model of complex differential driven robot with 2 rear wheels and 2 front wheels.
- scenarios: contains all scenarios that can be inserted in the world of main.sdf. A scenario is usually a composite of items, which can be models, to create a scene. A scene could be a table with a floor, 2 tables and 10 chairs.
    - obstacle_scenario: a scene with multiple obstacles, such as walls and rectangle blocks.
- main.sdf: the world where scenarios and models can be put in to create a vast amount of different results. You could place the same scenario and models multiple times in different places to create a different scene altogether.

> [!IMPORTANT]
> Be careful when inserting multiple models of robots. The topics could have the same names across multiple robots. For example: vehicle blue and x1 could have the same name for the imu topic and the topic that controls the robot (cmd_vel).


### Independent use
To use this package alone, follow the next instructions.
To use it with all the other packages, check the README.md of amr_launch.

1. Navigate to the root of your project
2. Build the project.

```
colcon build --packages-select amr_description
```

3. Source your project. 

```
. ./install/local_setup.bash
```

4. Run the launch file.

```
ros2 launch amr_description gazebo_launch.py
```

5. A simulated world in gazebo should open.