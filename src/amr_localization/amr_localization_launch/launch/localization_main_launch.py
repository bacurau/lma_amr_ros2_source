from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch_ros.substitutions import FindPackageShare
from launch.substitutions import  PathJoinSubstitution


def generate_launch_description():

    # Find and include localization launch files, for now just odometry
    share_localization_launch_path = FindPackageShare('amr_localization_launch')
    odometry_launch_file_path = PathJoinSubstitution([
            share_localization_launch_path, 
            'launch', 
            'odometry_launch.py'
        ])
    odometry_launch_file = IncludeLaunchDescription(odometry_launch_file_path)


    ## Create launch description and add actions, each action is a launch file or a node and
    ## will be executed in the order of addition.
    main_launch_description = LaunchDescription()
    main_launch_description.add_action(odometry_launch_file)

    
    return main_launch_description
