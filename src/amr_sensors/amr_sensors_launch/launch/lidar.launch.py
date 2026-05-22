from launch import LaunchDescription
from ament_index_python.packages import get_package_share_directory
from launch_ros.actions import Node
import os

def generate_launch_description():

    ld = LaunchDescription()

    sick_back = Node(
            package='sick_scan_xd',
            name='sick_scan_back_node',
            executable='sick_generic_caller',
            output='screen',
            arguments=[os.path.join(get_package_share_directory('amr_sensors_launch'), 'launch/secondary_launchs/sick_back.launch')] 
    )

    sick_front = Node(
            package='sick_scan_xd',
            name='sick_scan_front_node',
            executable='sick_generic_caller',
            output='screen',
            arguments=[os.path.join(get_package_share_directory('amr_sensors_launch'), 'launch/secondary_launchs/sick_front.launch')] 
    )

    ld.add_action(sick_back)
    ld.add_action(sick_front)

    return ld
