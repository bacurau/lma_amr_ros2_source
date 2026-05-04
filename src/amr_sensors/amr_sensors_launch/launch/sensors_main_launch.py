from launch import LaunchDescription
from launch.substitutions import PathJoinSubstitution
from launch.actions import IncludeLaunchDescription
from launch_ros.substitutions import FindPackageShare
from launch_ros.actions import Node
def generate_launch_description():

    
    share_launch_path = FindPackageShare('amr_sensors')
    launch_imu_file_path = PathJoinSubstitution([
            share_launch_path, 
            'launch', 
            'imu.launch.py'
        ])
    launch_lidar_file_path = PathJoinSubstitution([
            share_launch_path, 
            'launch', 
            'lidar.launch.py'
        ])
    
    launch_velodynes_file_path = PathJoinSubstitution([
            share_launch_path, 
            'launch', 
            'velodyne_launch.py'
        ])


    launch_imu = IncludeLaunchDescription(launch_imu_file_path)
    
    launch_lidar = IncludeLaunchDescription(launch_lidar_file_path)

    launch_velodynes = IncludeLaunchDescription(launch_velodynes_file_path)

    main_launch_description = LaunchDescription()
    main_launch_description.add_action(launch_imu)
    main_launch_description.add_action(launch_lidar)
    main_launch_description.add_action(launch_velodynes)

    return main_launch_description