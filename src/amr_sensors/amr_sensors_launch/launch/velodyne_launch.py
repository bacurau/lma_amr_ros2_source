from launch import LaunchDescription
from launch.substitutions import PathJoinSubstitution
from launch.actions import IncludeLaunchDescription
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():

    share_launch_path = FindPackageShare('amr_sensors_launch')
    launch_velodyne_file_path = PathJoinSubstitution([
            share_launch_path, 
            'launch/secondary_launchs', 
            'velodyne_VLP16_individual_launch.py'
        ])
    
    # the name of the namespaces have to be equal to the namespace names in the files ./params/velodyne_back_params.yaml and ./params/velodyne_front_params.yaml
    # /velodyne_front_right/velodyne_driver_node: -> velodyne_front_right
    # /velodyne_back_left/velodyne_driver_node: -> velodyne_back_left
    launch_velodyne_front = IncludeLaunchDescription(launch_velodyne_file_path, launch_arguments={'velodyne_ns': 'velodyne_front_right'}.items())
    launch_velodyne_back = IncludeLaunchDescription(launch_velodyne_file_path, launch_arguments={'velodyne_ns': 'velodyne_back_left'}.items())
    velodyne_launch_description = LaunchDescription()
    velodyne_launch_description.add_action(launch_velodyne_front)
    velodyne_launch_description.add_action(launch_velodyne_back)

    return velodyne_launch_description