from launch import LaunchDescription
from launch_ros.actions import Node
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.actions import IncludeLaunchDescription
import os
from ament_index_python.packages import get_package_share_directory
from pathlib import Path

def generate_launch_description():

    sensors_package_share_dir = get_package_share_directory('amr_sensors')
    description_package_share_dir = get_package_share_directory('amr_description')

    sensors_launch_file_path = os.path.join(sensors_package_share_dir, 'launch', 'imu.launch.py')
    urdf_file_path = os.path.join(description_package_share_dir, 'amr', 'urdf', 'amr.urdf')
    
    # create string variable with the contents of the urdf file
    with open(urdf_file_path, 'r') as f:
        urdf_xml = f.read()
    
    ld = LaunchDescription()
    
    sensors_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(sensors_launch_file_path)
    )

    robot_state_publisher_node = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        output='screen',
        parameters=[{'robot_description': urdf_xml}] # you have to pass a string with the contents of the urdf file
        #arguments=[urdf_file_path]
    )

    microROS_node = Node(
        package='micro_ros_agent',
        executable='micro_ros_agent',
        name='micro_ros_agent',
        #node_namespace='scooby',
        output='screen',
        arguments=["serial", "--dev", "/dev/ttyACM0", "-b", "1500000"]
        )
    
    reset_arm_node = Node(
        package='amr_serial',
        executable='reset_arm',
        name='reset_arm',
        #node_namespace='scooby',
        output='screen'
        )

    #ld.add_action(sensors_launch)
    ld.add_action(robot_state_publisher_node)
    ld.add_action(reset_arm_node)
    ld.add_action(microROS_node)
    
    return ld
