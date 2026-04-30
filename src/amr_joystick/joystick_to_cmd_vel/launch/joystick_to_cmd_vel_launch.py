from launch import LaunchDescription
from launch.substitutions import PathJoinSubstitution
from launch.actions import IncludeLaunchDescription
from launch_ros.substitutions import FindPackageShare
from launch_ros.actions import Node
def generate_launch_description():


    share_folder_path_for_joy = FindPackageShare('joystick_to_cmd_vel')
    config_joy_file_path = PathJoinSubstitution([
            share_folder_path_for_joy, 
            'config', 
            'joy-params.yaml'
    ])
 
    joy_node =  Node(package='joy',
                    executable='joy_node',
                    output='both',
                    parameters=[config_joy_file_path]
                )

    joystick_to_cmd_vel_node = Node(
        package='joystick_to_cmd_vel',
        executable='joystick_to_cmd_vel_node',
        name='joystick_to_cmd_vel_node', 
        output='screen',
    )
    


    main_launch_description = LaunchDescription()
    main_launch_description.add_action(joy_node)
    main_launch_description.add_action(joystick_to_cmd_vel_node)

    return main_launch_description