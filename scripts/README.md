# Instruction for scripts execution

This scripts were created to facilitate the build and execution of this ros2 project.

> [!IMPORTANT]  
> You have to be inside the docker container. The repository for the docker container is https://github.com/bacurau/lma_amr_ros2_docker_environment.


> [!IMPORTANT]  
> Give the scripts execution permission. 
> chmod +x main.sh
> chmod +x install_dependencies.sh
> chmod +x create_and_build_microROS_agent.sh
> chmod +x run_project.sh



Scripts:
- main.sh: installs the dependencies; builds ros2 project; creates and builds micro-ROS agent; change ROS_DOMAIN_ID to 25; and runs this ros2 project.

    - install_dependencies.sh: installs the ros2 project dependencies with rosdep.

    - create_and_build_microROS_agent.sh: Creates and builds the micro-ROS agent for the AMR project

    - run_project.sh: Changes ROS_DOMAIN_ID to 25 and runs amr ros2 project.


The main.sh script calls the other scripts. After the first execution of main.sh, to re-run the project, just execute run_project.sh.