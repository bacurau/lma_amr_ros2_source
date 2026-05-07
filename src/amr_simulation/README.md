## Summary

This folder/package is responsible for executing the gazebo simulation.
The user can edit the file worlds/world.sdf.xacro to create his own world. 
> [!IMPORTANT]
> The robot models is not on this folder, it is on the package/folder amr_description.

Structure:
- launch: responsible for launching the gazebo simulation with the chosen world file.
- models: contains all the models used by the simulation. A model is usually a single item, such as a robot, a chair, a table, a wall, etc.
    - vehicle_blue: simplest model of a differential driven robot with 2 rear wheels and a caster wheel in the front.
    - wall: model for a wall.
    - x1: model of complex differential driven robot with 2 rear wheels and 2 front wheels.
- scenarios: contains all scenarios that can be inserted in a world. A scenario is usually a composite of items, which can be models, to create a scene. A scene could be a table with a floor, 2 tables and 10 chairs.
    - obstacle_scenario: a scene with multiple obstacles, such as walls and rectangle blocks.
- worlds: contains all the worlds created.
    - world.sdf.xacro: a sdf xacro file of a sdf world where scenarios and models can be put in to create a vast amount of different results. You could place the same scenario and models multiple times in different places to create a different scene altogether.
    - world.sdf: the result of converting world.sdf.xacro to sdf format.

> [!IMPORTANT]
> Be careful when inserting multiple models of robots. The topics could have the same names across multiple robots. For example: vehicle blue and x1 could have the same name for the imu topic and the topic that controls the robot (cmd_vel).



## Generate sdf from xacro

The conversion is done directly by the launch file, but the command used is:

```
ros2 run xacro xacro world2.sdf.xacro > temp.sdf
```

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
ros2 launch amr_simulation simulation_launch.py
```

5. A simulated world in gazebo should open.