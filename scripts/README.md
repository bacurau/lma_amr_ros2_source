# Instruction for scripts execution

This scripts were created to facilitate the build and execution of this ros2 project.
This script opens a menu that allows the user to:

    1. Build and then run the project **OR** just run a project you have already built.
    2. Run the real robot or the simulation.

Scripts:
- main.bash: installs the dependencies; builds ros2 project; creates and builds micro-ROS agent; change ROS_DOMAIN_ID to 25; and runs this ros2 project.


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
./main.sh [ Type of build (default Release) ]
```
> [!NOTE]
> [] indicates optional argument.

Examples:
- ```./main.sh ``` if you choose to build, it will build in the standard mode (Release).
- ```./main.sh Debug```if you choose to build, it will build in the debugger mode (Debug).
- ```./main.sh Release``` if you choose to build, it will build in the standard mode (Release).


After the first execution of main.sh, to re-run the project, just execute run_project.sh.
```bash
./run_project.sh
```





## Notes
1. Before any command is executed the scripts navigates to the root of the workspace. Properly executing the ros2 commands in the correct folder.

```bash
cd .. # Navigate to the root of the workspace, so you can install the dependencies and build ros2 packages properly.
```

2. Remember that if you change the code, you have to execute the command
```bash
colcon build
```
For the python files, ```colcon build --symlink-install``` ensures that you don't need to build again for the alterations to take effect.
