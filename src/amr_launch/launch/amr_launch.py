from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import IncludeLaunchDescription
import os
from ament_index_python.packages import get_package_share_directory
from launch_ros.substitutions import FindPackageShare
from launch.substitutions import  PathJoinSubstitution
from launch_ros.actions import SetParameter
def generate_launch_description():

    #================= Find and include stm32 launch file ==============================================================
    share_stm32_launch_path = FindPackageShare('amr_stm32_launch')
    stm32_launch_file_path = PathJoinSubstitution([
            share_stm32_launch_path, 
            'launch', 
            'stm32_launch.xml'
        ])
    stm32_launch_file = IncludeLaunchDescription(stm32_launch_file_path)


    #================= Find and include sensors main launch file ============================================================================
    share_sensors_launch_path = FindPackageShare('amr_sensors_launch')
    sensors_launch_file_path = PathJoinSubstitution([
            share_sensors_launch_path, 
            'launch', 
            'sensors_main_launch.py'
        ])
    sensors_launch_file = IncludeLaunchDescription(sensors_launch_file_path)

    # ====================== Creates robot_state_publisher node ==============================================================================
 

    # Using os.path.join returns a string with the filepath, which is needed for reading the file.
    description_package_share_dir = get_package_share_directory('amr_description')
    urdf_file_path = os.path.join(description_package_share_dir, 'amr', 'urdf', 'amr.urdf')
    
    # Creates string variable with the contents of the urdf file
    with open(urdf_file_path, 'r') as f:
        real_robot_urdf = f.read()

    # this node takes the urdf or sdf file passed to created a ros2 tf tree.    
    robot_state_publisher_node = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher_node', 
        output='screen',
        parameters=[{'robot_description': real_robot_urdf}] # you have to pass a string with the contents of the urdf file
    )

    #====================== Find and include localization main launch file====================================================================
    share_localization_launch_path = FindPackageShare('amr_localization_launch')
    localization_launch_file_path = PathJoinSubstitution([
            share_localization_launch_path, 
            'launch', 
            'localization_main_launch.py'
        ])
    localization_launch_file = IncludeLaunchDescription(localization_launch_file_path)


    #======================= Find and include joystick main launch file=======================================================================
    # The joystick, if connected to the laptop, will send velocities through the cmd_vel topic.
    # Note: Every joystick has a different mapping of buttons, please alter the joystick_to_cmd_vel accordingly.
    share_folder_path_for_joystick_to_cmd_vel_ = FindPackageShare('joystick_to_cmd_vel')
    joystick_to_cmd_vel_launch_file_path = PathJoinSubstitution([
            share_folder_path_for_joystick_to_cmd_vel_, 
            'launch', 
            'joystick_to_cmd_vel_launch.py'
        ])
    joystick_to_cmd_vel_launch_file = IncludeLaunchDescription(joystick_to_cmd_vel_launch_file_path)




    #======================== Create rviz2 node for visualizing localization results ==========================================================
    rviz2_node = Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2_node',
            arguments=['-d', PathJoinSubstitution([share_localization_launch_path, 'rviz/odometry.rviz'])],
            output='screen'
    )

    #=========================================================================================================================================


    # Find and include localization main launch file
    share_folder_path_for_robot_localization = FindPackageShare('robot_localization')
    robot_localization_launch_file_path = PathJoinSubstitution([
            share_folder_path_for_robot_localization, 
            'launch', 
            'ekf.launch.py'
        ])
    robot_localization_launch_file = IncludeLaunchDescription(robot_localization_launch_file_path)



    use_sim_time = SetParameter(name='use_sim_time', value=True)




    ## Add action to the launch. Each action is a launch file or a node and will be executed in the order they were added.
    main_launch_description = LaunchDescription() ## Create launch description
    main_launch_description.add_action(use_sim_time)
    #main_launch_description.add_action(robot_state_publisher_node)
    #main_launch_description.add_action(sensors_launch_file)
    #main_launch_description.add_action(stm32_launch_file)
    main_launch_description.add_action(localization_launch_file)
    #main_launch_description.add_action(joystick_to_cmd_vel_launch_file)
    main_launch_description.add_action(rviz2_node)
    main_launch_description.add_action(robot_localization_launch_file)
    return main_launch_description
