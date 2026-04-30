from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import IncludeLaunchDescription
import os
from ament_index_python.packages import get_package_share_directory
from launch_ros.substitutions import FindPackageShare
from launch.substitutions import  PathJoinSubstitution


def generate_launch_description():

    # Find and include stm32 launch file
    share_stm32_launch_path = FindPackageShare('stm32_launch')
    stm32_launch_file_path = PathJoinSubstitution([
            share_stm32_launch_path, 
            'launch', 
            'stm32_launch.xml'
        ])
    stm32_launch_file = IncludeLaunchDescription(stm32_launch_file_path)

    # Find and include sensors main launch file
    share_sensors_launch_path = FindPackageShare('amr_sensors')
    sensors_launch_file_path = PathJoinSubstitution([
            share_sensors_launch_path, 
            'launch', 
            'sensors_main_launch.py'
        ])
    sensors_launch_file = IncludeLaunchDescription(sensors_launch_file_path)


    # Find and include localization main launch file
    share_localization_launch_path = FindPackageShare('amr_localization_launch')
    localization_launch_file_path = PathJoinSubstitution([
            share_localization_launch_path, 
            'launch', 
            'localization_main_launch.py'
        ])
    localization_launch_file = IncludeLaunchDescription(localization_launch_file_path)



    # Find robot description urdf file, 
    # Using os.path.join returns a string with the filepath,
    # which is needed for reading the file.
    description_package_share_dir = get_package_share_directory('amr_description')
    urdf_file_path = os.path.join(description_package_share_dir, 'amr', 'urdf', 'amr.urdf')
    
    # create string variable with the contents of the urdf file
    with open(urdf_file_path, 'r') as f:
        urdf_xml = f.read()
    
    ## Create robot_state_publisher node
    robot_state_publisher_node = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher_node', 
        output='screen',
        parameters=[{'robot_description': urdf_xml}] # you have to pass a string with the contents of the urdf file
        #arguments=[urdf_file_path]
    )


     # Find and include localization main launch file
    share_folder_path_for_joystick_to_cmd_vel_ = FindPackageShare('joystick_to_cmd_vel')
    joystick_to_cmd_vel_launch_file_path = PathJoinSubstitution([
            share_folder_path_for_joystick_to_cmd_vel_, 
            'launch', 
            'joystick_to_cmd_vel_launch.py'
        ])
    joystick_to_cmd_vel_launch_file = IncludeLaunchDescription(joystick_to_cmd_vel_launch_file_path)


    ## Create launch description and add actions, each action is a launch file or a node and
    ## will be executed in the order of addition.
    main_launch_description = LaunchDescription()
    main_launch_description.add_action(robot_state_publisher_node)
    main_launch_description.add_action(sensors_launch_file)
    main_launch_description.add_action(stm32_launch_file)
    main_launch_description.add_action(localization_launch_file)
    #main_launch_description.add_action(joystick_to_cmd_vel_launch_file)
    
    
    return main_launch_description
