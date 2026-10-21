# Instruction for scripts execution

This scripts were created to facilitate the build and execution of this ros2 project.

> [!IMPORTANT]  
> You have to be inside the docker container. The repository for the docker container is https://github.com/bacurau/lma_amr_ros2_docker_environment.


> [!IMPORTANT]  
> Give the scripts execution permission. 
> - ```chmod +x main.bash``` 
> - ```chmod +x config.bash```
> - ```chmod +x play_bag.bash```
> - ```chmod +x record_bag.bash```
> - ```chmod +x signal_handler.bash```


> [!WARNING]  
> Executing the scripts with "." or "source" can close the vscode terminal after the first execution.
> Example: ``` source ./main.sh ``` or ```. ./main.sh```.
> It is not clear why this happens.


## Execution

1- Choose your configuration in the config.bash file.
A shortened configuration example to build the project, build micro-ros, and then run the real robot:

```
export BUILD_PROJECT=true 
export RUN_PROJECT=true
export USE_SIMULATION=false 
export BUILD_MICROROS=true
```

2- Execute:
```bash
./main.bash
```
## Organization

Scripts:
- main.bash: installs the dependencies; builds ros2 project; creates and  builds micro-ROS agent; 
change ROS_DOMAIN_ID to 25; and runs this ros2  project. Change options in the config.bash file.
- config.bash: configuration file. This file is used to determine if you are going to build, run, use simulation and other options for your ros2 project.
- play_bag.bash: starts the ros2 project and plays a ros2 bag of your choosing.
- record_bag.bash: starts the ros2 project and records a ros2 bag.
- signal_handler.bash: creates handlers for signals.



## Notes
1. Remember that if you change the ros2 code (ros2 packages), you have to execute the command
```bash
colcon build
```
For the python files, ```colcon build --symlink-install``` ensures that you don't need to build again for the alterations to take effect.
