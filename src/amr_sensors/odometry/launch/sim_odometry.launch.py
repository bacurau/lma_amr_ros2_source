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

"""Launch motors node."""

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
        parameters=[{'use_sim_time': True}],
        # remappings=[
        #     ("joint_states", "joint_state")
        # ],
        arguments=[urdf_file])

    start_scooby_teleop_cmd = Node(
        condition=IfCondition(use_teleop),
        package='scooby_teleop',
        executable='scooby_teleop',
        name='scooby_teleop',
        remappings=[
            ("/scooby/cmd_vel", "/cmd_vel")
        ],
        #output='screen',
        parameters=[{'use_sim_time': True}],
        )

    # rviz = Node(
    #     package='rviz2',
    #     executable='rviz2',
    #     arguments=['-d', os.path.join(scooby_node_pkg, 'rviz', 'node.rviz')],
    #     condition=IfCondition(use_rviz)
    # )

    odom_node = Node(
        package='motors_node',
        executable='snoopy_ros',
        name='scooby_node',
        #node_namespace='scooby',
        output='screen',
        parameters=[os.path.join(scooby_node_pkg, 'params', 'scooby.yaml')]
        )
    
        
    ld = LaunchDescription([
        DeclareLaunchArgument('use_rviz', default_value='false',
                              description='Open RViz.'),
        DeclareLaunchArgument('use_teleop', default_value='false',
                              description='Open scooby_teleop'),
        DeclareLaunchArgument('urdf_file',default_value=os.path.join(snoopy_description_pkg, 'urdf', 'snoopy.urdf'),
                              description='urddeclare_urdf_cmd =f file complete path'),
        DeclareLaunchArgument('teleop_config', default_value=parameters_file),
        odom_node,
        # start_scooby_teleop_cmd,
        # robot_state_publisher,
    ])
            
    return ld
