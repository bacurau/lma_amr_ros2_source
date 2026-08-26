from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import IncludeLaunchDescription
import os
from ament_index_python.packages import get_package_share_directory
from launch_ros.substitutions import FindPackageShare
from launch.substitutions import  PathJoinSubstitution
from launch_ros.actions import SetParameter

def generate_launch_description():

   
    # Find and include localization main launch file
    share_localization_launch_path = FindPackageShare('amr_localization_launch')
    localization_launch_file_path = PathJoinSubstitution([
            share_localization_launch_path, 
            'launch', 
            'localization_main_launch.py'
        ])
    localization_launch_file = IncludeLaunchDescription(localization_launch_file_path)

    # Find and include localization main launch file
    share_folder_path_for_joystick_to_cmd_vel_ = FindPackageShare('joystick_to_cmd_vel')
    joystick_to_cmd_vel_launch_file_path = PathJoinSubstitution([
            share_folder_path_for_joystick_to_cmd_vel_, 
            'launch', 
            'joystick_to_cmd_vel_launch.py'
        ])
    joystick_to_cmd_vel_launch_file = IncludeLaunchDescription(joystick_to_cmd_vel_launch_file_path)

    # Find and include localization main launch file
    share_folder_path_for_gz_sim_bringup = FindPackageShare('gz_sim_bringup')
    gz_sim_bringup_launch_file_path = PathJoinSubstitution([
            share_folder_path_for_gz_sim_bringup, 
            'launch', 
            'simulation_launch.py'
        ])
    gz_sim_bringup_launch_file = IncludeLaunchDescription(gz_sim_bringup_launch_file_path)
  
    # Find robot description urdf file, 
    # Using os.path.join returns a string with the filepath,
    # which is needed for reading the file.
    gz_sim_description_package_share_dir = get_package_share_directory('gz_sim_description')
    vehicle_blue_sdf_file_path = os.path.join(gz_sim_description_package_share_dir, 'models', 'diff_drive', 'model.sdf')
    
    # create string variable with the contents of the urdf file
    with open(vehicle_blue_sdf_file_path, 'r') as f:
        vehicle_blue_sdf = f.read()
    
    ## Create robot_state_publisher node (used for the ros2 tfs)
    vehicle_blue_state_publisher_node_sdf = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher_node', 
        output='screen',
        parameters=[{'robot_description': vehicle_blue_sdf}] # you have to pass a string with the contents of the urdf file
    )

    # https://robotics.stackexchange.com/questions/97957/ros2-set-parameters-from-file-and-directly
    use_sim_time = SetParameter(name='use_sim_time', value=True)

    #===========================================================================================================================================

    ## Create launch description and add actions, each action is a launch file or a node and
    ## will be executed in the order of addition.
    main_launch_description = LaunchDescription()
    main_launch_description.add_action(use_sim_time)
    main_launch_description.add_action(localization_launch_file)
    main_launch_description.add_action(joystick_to_cmd_vel_launch_file)
    main_launch_description.add_action(gz_sim_bringup_launch_file)
    main_launch_description.add_action(vehicle_blue_state_publisher_node_sdf)
    
    return main_launch_description
