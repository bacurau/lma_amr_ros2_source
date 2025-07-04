from launch import LaunchDescription
from ament_index_python.packages import get_package_share_directory
from launch_ros.actions import Node
import os

def generate_launch_description():

    ld = LaunchDescription()
    sick_scan_pkg_prefix = get_package_share_directory('amr_sensors')
    launch_file_path_back = os.path.join(sick_scan_pkg_prefix, 'launch/sick_back.launch') 
    launch_file_path_front = os.path.join(sick_scan_pkg_prefix, 'launch/sick_front.launch') 
    node_arguments_back=[launch_file_path_back]
    node_arguments_front=[launch_file_path_front]

    sick_back = Node(
            package='sick_scan_xd',
            executable='sick_generic_caller',
            output='screen',
            arguments=node_arguments_back
    )

    sick_front = Node(
            package='sick_scan_xd',
            executable='sick_generic_caller',
            output='screen',
            arguments=node_arguments_front
    )

    ld.add_action(sick_back)
    ld.add_action(sick_front)

    return ld
