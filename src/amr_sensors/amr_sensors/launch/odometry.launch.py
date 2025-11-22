from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import IncludeLaunchDescription
import os
from ament_index_python.packages import get_package_share_directory
from launch_ros.substitutions import FindPackageShare
from launch.substitutions import  PathJoinSubstitution

def generate_launch_description():

    ld = LaunchDescription()

    share_motors_node_path = FindPackageShare('motors_node')
    motors_param_file_path = PathJoinSubstitution([
            share_motors_node_path, 
            'param', 
            'snoopy.yaml'
        ])
    #motors_param_file = IncludeLaunchDescription(motors_param_file_path)


    odom_node = Node(
        package='motors_node',
        executable='snoopy_ros',
        name='snoopy_node',
        #node_namespace='scooby',
        output='screen',
        parameters=[motors_param_file_path],
        #remappings = [('odom', 'motor_odom')]
        )
    ld.add_action(odom_node)

    return ld
