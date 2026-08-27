from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
import os
from ament_index_python.packages import get_package_share_directory
from launch_ros.substitutions import FindPackageShare
from launch.substitutions import  LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import SetParameter
from launch.conditions import IfCondition,UnlessCondition

def generate_launch_description():

    run_launch_with_bags = LaunchConfiguration('run_launch_with_bags')

    run_launch_with_bags_argument = DeclareLaunchArgument(
          'run_launch_with_bags',
          default_value='true',
          description='Whether to run a launch file'
    )
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
    joystick_to_cmd_vel_launch_file = IncludeLaunchDescription(joystick_to_cmd_vel_launch_file_path,condition=UnlessCondition(run_launch_with_bags))

    # Find and include localization main launch file
    share_folder_path_for_amr_simulation = FindPackageShare('amr_simulation')
    amr_simulation_launch_file_path = PathJoinSubstitution([
            share_folder_path_for_amr_simulation, 
            'launch', 
            'simulation_launch.py'
        ])
    amr_simulation_launch_file = IncludeLaunchDescription(amr_simulation_launch_file_path)
  
    # Find robot description urdf file, 
    # Using os.path.join returns a string with the filepath,
    # which is needed for reading the file.
    description_package_share_dir = get_package_share_directory('amr_simulation')
    vehicle_blue_sdf_file_path = os.path.join(description_package_share_dir, 'models', 'vehicle_blue', 'model.sdf')
    
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

    #======================== Create rviz2 node for visualizing localization results ==========================================================
    rviz2_node = Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2_node',
            arguments=['-d', PathJoinSubstitution([share_localization_launch_path, 'rviz/odometry.rviz'])],
            output='screen'
    )



    ## Create launch description and add actions, each action is a launch file or a node and
    ## will be executed in the order of addition.
    main_launch_description = LaunchDescription()
    main_launch_description.add_action(use_sim_time)
    main_launch_description.add_action(run_launch_with_bags_argument)
    main_launch_description.add_action(localization_launch_file)
    main_launch_description.add_action(joystick_to_cmd_vel_launch_file)
    main_launch_description.add_action(amr_simulation_launch_file)
    main_launch_description.add_action(vehicle_blue_state_publisher_node_sdf)
    main_launch_description.add_action(rviz2_node)
    
    

    
    
    return main_launch_description
