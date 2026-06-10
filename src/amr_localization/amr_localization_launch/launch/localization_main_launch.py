from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription,DeclareLaunchArgument
from launch_ros.substitutions import FindPackageShare
from launch.substitutions import  PathJoinSubstitution, LaunchConfiguration
from launch_ros.actions import Node, LifecycleNode

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
            #prefix='gdbserver localhost:3000',
            package='change_covariance_value',
            executable='imu_covariance',
            name='imu_covariance',
            output='screen'
    )


    autostart = DeclareLaunchArgument(
        'autostart',
        default_value='true',
        description='Automatically configure and activate the node. Set to false for managed lifecycle control.')

    ekf_node = LifecycleNode(
        package='robot_localization',
        executable='ekf_node',
        name='ekf_filter_node',
        namespace='',
        output='screen',
        autostart=LaunchConfiguration('autostart'),
        parameters=[PathJoinSubstitution([FindPackageShare('amr_localization_launch'),'params','ekf.yaml'])]
    )




    ## Create launch description and add actions, each action is a launch file or a node and
    ## will be executed in the order of addition.
    main_launch_description = LaunchDescription()
    main_launch_description.add_action(autostart)
    main_launch_description.add_action(odometry_launch_file)
    main_launch_description.add_action(imu_cov_node)
    main_launch_description.add_action(ekf_node)

    
    return main_launch_description
