from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import SetEnvironmentVariable, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution,EnvironmentVariable
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():
    ros_gz_sim_pkg_path = get_package_share_directory('ros_gz_sim')
    amr_description_pkg_path = FindPackageShare('amr_description')  # Replace with your own package name
    gz_launch_path = PathJoinSubstitution([ros_gz_sim_pkg_path, 'launch', 'gz_sim.launch.py'])
    return LaunchDescription([
        SetEnvironmentVariable(
            'GZ_SIM_RESOURCE_PATH',[
            PathJoinSubstitution([amr_description_pkg_path, 'models']),
            ':',
            PathJoinSubstitution([amr_description_pkg_path, 'worlds'])
            ]
        ),
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(gz_launch_path),
            launch_arguments={
                'gz_args': [PathJoinSubstitution([amr_description_pkg_path, 'worlds/main.sdf -r'])], 
                            # '--gui-config ', 
                            # PathJoinSubstitution([example_pkg_path, 'config/gazebo_config_with_keyboard_press.config '])],
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
        ),
    ])