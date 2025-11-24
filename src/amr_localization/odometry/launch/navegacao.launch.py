# Copyright 2021 Olmer Garcia
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""Launch Gazebo with a world that has Scooby, as well as the follow node."""

import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.actions import IncludeLaunchDescription,TimerAction
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():

    snoopy_description_pkg=get_package_share_directory('snoopy_v2_description')
    scooby_node_pkg=get_package_share_directory('motors_node')
    use_robot_state_pub = LaunchConfiguration('use_robot_state_pub')
    use_rviz = LaunchConfiguration('use_rviz')
    use_teleop = LaunchConfiguration('use_teleop')
    urdf_file= LaunchConfiguration('urdf_file')
    parameters_file = os.path.join(
        scooby_node_pkg,
        'param', 'joy_teleop.yaml'
    )


    robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        output='screen',
        parameters=[{'use_sim_time': False}],
        remappings=[
            ("joint_states", "joint_states_jetson")
        ],
        arguments=[urdf_file])
    
    imu_cmd = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(os.path.join(get_package_share_directory('xsens_mti_driver'), 'launch'),
             'xsens_mti_node.launch.py')),
        #launch_arguments={'namespace': namespace}.items()
        )
      
    velodyne_cmd_b = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(os.path.join(get_package_share_directory('velodyne'), 'launch'),
             'b_velodyne-all-nodes-VLP16-launch.py')),
        #launch_arguments={'namespace': namespace}.items()
        )
    velodyne_cmd_c = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(os.path.join(get_package_share_directory('velodyne'), 'launch'),
             'c_velodyne-all-nodes-VLP16-launch.py')),
        #launch_arguments={'namespace': namespace}.items()
        )
    odom_node = Node(
        package='motors_node',
        executable='snoopy_ros',
        name='snoopy_node',
        #node_namespace='scooby',
        output='screen',
        parameters=[os.path.join(scooby_node_pkg, 'param', 'snoopy.yaml')]
        )

    #    ros2 run scooby_serial reset_arm
    reset_arm_node = Node(
        package='scooby_serial',
        executable='reset_arm',
        name='reset_arm',
        #node_namespace='scooby',
        output='screen'
        )


    static_transform = Node(package = "tf2_ros", 
                       executable = "static_transform_publisher",
                       arguments = ["0.0", "0.0", "0.", "0.0", "0", "0.0", "map", "odom"])
                    #    arguments = ["-3.8781", "-1.8797", "0.", "0.0", "0", "0.0", "map", "odom"]

    #ros2 launch scooby_cartographer cartographer.launch.py

    #  ros2 launch localization_validations fusion_experiment.launch.py
    ekf_cmd = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(get_package_share_directory('localization_validations'),'launch' ,'fusion_experiment.launch.py')),
            launch_arguments={'use_sim_time': 'false'}.items()
        )      
    #ros2 launch snoopy_nav2_sim fem_snoopy_launch.py
    nav2_cmd = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(get_package_share_directory('snoopy_nav2_sim'),'launch' ,'fem_snoopy_launch.py')),
            launch_arguments={'use_sim_time': 'false'}.items()
        )
    # ros2 launch wp_follower_action_client snoopy_rviz_wp_follower_launch.py
    waypoint_cmd = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(get_package_share_directory('wp_follower_action_client'),'launch' ,'snoopy_rviz_wp_follower_launch.py')),
            launch_arguments={'use_sim_time': 'false'}.items()
        )      

    # ros2 run laser_docking laser_docking_node
    laser_docking_node = Node(
        package='laser_docking',
        executable='laser_docking_node',
        name='laser_docking_node',
        #node_namespace='scooby',
        output='screen'
        )

    ld = LaunchDescription([
        DeclareLaunchArgument('use_sim_time',default_value='false',
                            description='Use simulation clock if true'),
        DeclareLaunchArgument('use_rviz', default_value='false',
                              description='Open RViz.'),
        DeclareLaunchArgument('use_teleop', default_value='false',
                              description='Open scooby_teleop'),
        DeclareLaunchArgument('urdf_file',default_value=os.path.join(snoopy_description_pkg, 'urdf', 'snoopy.urdf'),
                              description='urddeclare_urdf_cmd =f file complete path'),                   
        #DeclareLaunchArgument('cmd_vel', default_value='scooby/cmd_vel'),
        DeclareLaunchArgument('teleop_config', default_value=parameters_file),
        odom_node,
        reset_arm_node,
        imu_cmd,
        velodyne_cmd_b,
        velodyne_cmd_c,
        robot_state_publisher,
        static_transform,
        laser_docking_node,
        # ekf_cmd,
        TimerAction(                                         
            period="5",                                                     
            actions=[ 
                nav2_cmd,                                                    
                waypoint_cmd      
            ]                                                               
        )
        #errors_node
    ])
    return ld
