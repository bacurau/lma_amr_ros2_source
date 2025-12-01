from launch import LaunchDescription
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from launch.substitutions import  PathJoinSubstitution

def generate_launch_description():

    ld = LaunchDescription()

    share_motors_node_path = FindPackageShare('odometry')
    motors_param_file_path = PathJoinSubstitution([
            share_motors_node_path, 
            'param', 
            'snoopy.yaml'
        ])
    #motors_param_file = IncludeLaunchDescription(motors_param_file_path)


    odom_node = Node(
        package='odometry',
        executable='odometry_node',
        name='odometry_node',
        output='screen',
        parameters=[motors_param_file_path],
        )

    ld.add_action(odom_node)

    return ld
