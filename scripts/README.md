# Instruction for scripts execution

This scripts were created to facilitate the build and execution of this ros2 project.

> [!IMPORTANT]  
> You have to be inside the docker container. The repository for the docker container is https://github.com/bacurau/lma_amr_ros2_docker_environment.


> [!IMPORTANT]  
> Give the scripts execution permission. 
> - ```chmod +x main.bash``` 
> - ```chmod +x install_dependencies.bash```
> - ```chmod +x create_and_build_microROS_agent.bash```
> - ```chmod +x run_project.bash```


> [!WARNING]  
> Executing the scripts with "." or "source" can close the vscode terminal after the first execution.
> Example: ``` source ./main.sh ``` or ```. ./main.sh```.
> It is not clear why this happens.


## Execution

First execution:
```bash
./main.sh [0: To use simulation | 1: To use the actual robot (default 1)] [ Type of build (default Release) ] [1:for not running after building  | 0: To run after building (default 0)]
```
> [!NOTE]
> [] indicates optional argument.

Examples:
- ```./main.sh ``` builds in the standard mode (Release) and then runs.
- ```./main.sh 0 ``` builds in the standard mode (Release), and then runs in simulation mode.
- ```./main.sh 1 1 Debug```builds in Debug mode, but does not run the project.
- ```./main.sh 1 1 Release``` builds in the standard mode (Release), but does not run the project..




After the first execution of main.sh, to re-run the project, just execute run_project.sh.
```bash
./run_project.sh
```

## Organization

Scripts:
- main.bash: installs the dependencies; builds ros2 project; creates and builds micro-ROS agent; change ROS_DOMAIN_ID to 25; and runs this ros2 project.

    - install_dependencies.bash: installs the ros2 project dependencies with rosdep.

    - create_and_build_microROS_agent.bash: Creates and builds the micro-ROS agent for the AMR project

    - run_project.bash: Changes ROS_DOMAIN_ID to 25 and runs amr ros2 project.



The main.bash script calls the other scripts. 




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
