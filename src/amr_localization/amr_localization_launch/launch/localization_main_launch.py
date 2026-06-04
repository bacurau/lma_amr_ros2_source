from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch_ros.substitutions import FindPackageShare
from launch.substitutions import  PathJoinSubstitution
from launch_ros.actions import Node

def generate_launch_description():

    # Find and include localization launch files, for now just odometry
    share_odometry_launch_path = FindPackageShare('odometry')
    odometry_launch_file_path = PathJoinSubstitution([
            share_odometry_launch_path, 
            'launch', 
            'odometry_launch.py'
        ])
    odometry_launch_file = IncludeLaunchDescription(odometry_launch_file_path)


    imu_cov_node = Node(
            package='change_covariance_value',
            executable='imu_covariance',
            name='imu_covariance',
            output='screen'
    )


    ## Create launch description and add actions, each action is a launch file or a node and
    ## will be executed in the order of addition.
    main_launch_description = LaunchDescription()
    main_launch_description.add_action(odometry_launch_file)
    main_launch_description.add_action(imu_cov_node)

    
    return main_launch_description
