# Instruction for scripts execution

This scripts were created to facilitate the build and execution of this ros2 project.
This script opens a menu that allows the user to:

    1. Build and then run the project **OR** just run a project you have already built.
    2. Run the real robot or the simulation.

Scripts:
- main.bash: installs the dependencies; builds ros2 project; creates and builds micro-ROS agent; change ROS_DOMAIN_ID to 25; and runs this ros2 project.
- config.bash: configuration file. Used to determine if you are going to build, run, use simulation and other options for your ros2 project.
- play_bag.bash: starts the ros2 project and plays a ros2 bag of your choosing.
- record_bag.bash: starts the ros2 project and records a ros2 bag.

> [!IMPORTANT]  
> You have to be inside the docker container. The repository for the docker container is https://github.com/bacurau/lma_amr_ros2_docker_environment.

> [!IMPORTANT]  
> Give the scripts execution permission. 
> - ```chmod +x main.bash``` 


> [!WARNING]  
> Executing the scripts with "." or "source" can close the vscode terminal after the first execution.
> Example: ``` source ./main.sh ``` or ```. ./main.sh```.
> It is not clear why this happens.


## Execution

First execution:
```bash
./main.sh
```

## Notes
1. Remember that if you change the code, you have to execute the command
```bash
colcon build
```
For the python files, ```colcon build --symlink-install``` ensures that you don't need to build again for alterations to take effect.
