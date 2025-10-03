from launch import LaunchDescription
from launch_ros.actions import Node
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.actions import IncludeLaunchDescription
import os
from ament_index_python.packages import get_package_share_directory
from pathlib import Path
from launch_ros.substitutions import FindPackageShare
from launch.substitutions import  PathJoinSubstitution


def generate_launch_description():

    sensors_package_share_dir = get_package_share_directory('amr_sensors')
    description_package_share_dir = get_package_share_directory('amr_description')

    sensors_launch_file_path = os.path.join(sensors_package_share_dir, 'launch', 'imu.launch.py')
    urdf_file_path = os.path.join(description_package_share_dir, 'amr', 'urdf', 'amr.urdf')
    
   
    sensors_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(sensors_launch_file_path)
    )

    share_stm32_launch_path = FindPackageShare('stm32_launch')
    reset_and_microRos_nodes_launch_file_path = PathJoinSubstitution([
            share_stm32_launch_path, 
            'launch', 
            'stm32_launch.xml'
        ])
    reset_and_microRos_nodes_launch_file = IncludeLaunchDescription(reset_and_microRos_nodes_launch_file_path)



    # create string variable with the contents of the urdf file
    with open(urdf_file_path, 'r') as f:
        urdf_xml = f.read()
    
    robot_state_publisher_node = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        output='screen',
        parameters=[{'robot_description': urdf_xml}] # you have to pass a string with the contents of the urdf file
        #arguments=[urdf_file_path]
    )


    main_launch_description = LaunchDescription()
    main_launch_description.add_action(robot_state_publisher_node)
    main_launch_description.add_action(reset_and_microRos_nodes_launch_file)
    
    return main_launch_description
