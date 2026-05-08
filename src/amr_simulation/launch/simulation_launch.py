from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import SetEnvironmentVariable, IncludeLaunchDescription,ExecuteProcess
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution,FindExecutable
from launch_ros.substitutions import FindPackageShare
import subprocess
# This launch was based on the launch present at the link https://gazebosim.org/docs/harmonic/ros2_launch_gazebo/

def generate_launch_description():
    ros_gz_sim_pkg_path = get_package_share_directory('ros_gz_sim')
    amr_description_pkg_path = FindPackageShare('amr_description')  #
    amr_simulation_pkg_path = FindPackageShare('amr_simulation')  #
    gz_launch_path = PathJoinSubstitution([ros_gz_sim_pkg_path, 'launch', 'gz_sim.launch.py'])

    return LaunchDescription([
        # this process runs the command 'ros2 run xacro xacro path/to/world.sdf.xacro > path/to/world.sdf'.
        # Used to convert the file from xacro to sdf.
        ExecuteProcess(
            cmd=[[
                FindExecutable(name='ros2'),
                ' run xacro xacro ',
                PathJoinSubstitution([amr_simulation_pkg_path, 'worlds/world.sdf.xacro']),
                '>',
                PathJoinSubstitution([amr_simulation_pkg_path, 'worlds/world.sdf']),
            ]],
            shell=True
        ),
        # needed so models and scenarios can be found using model://name in the sdf file.
        SetEnvironmentVariable(
            'GZ_SIM_RESOURCE_PATH',[
            PathJoinSubstitution([amr_simulation_pkg_path, 'models']),
            ':',
            PathJoinSubstitution([amr_simulation_pkg_path, 'scenarios']),
            ':',
            amr_description_pkg_path
            ]
        ),

        # To launch gazebo simulation.
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(gz_launch_path),
            launch_arguments={
                'gz_args': [PathJoinSubstitution([amr_simulation_pkg_path, 'worlds/world.sdf -r'])], 
                'on_exit_shutdown': 'True'
            }.items(),
        ),

        # Bridging and remapping Gazebo topics to ROS 2 (replace with your own topics)
        # https://github.com/gazebosim/ros_gz/blob/jazzy/ros_gz_bridge/README.md
        Node(
            package='ros_gz_bridge',
            executable='parameter_bridge',
            arguments=['/cmd_vel@geometry_msgs/msg/Twist@gz.msgs.Twist','/imu@sensor_msgs/msg/Imu@gz.msgs.IMU'],
            output='screen'
        )
    ])