import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import (
    DeclareLaunchArgument,
    ExecuteProcess,
    IncludeLaunchDescription,
    RegisterEventHandler,
    SetEnvironmentVariable,
)
from launch.conditions import IfCondition
from launch.event_handlers import OnExecutionComplete, OnProcessExit
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import FindExecutable, LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare

# This launch was based on the launch present at the link https://gazebosim.org/docs/harmonic/ros2_launch_gazebo/

def generate_launch_description():

    # ===============================================Find packages and launch files=====================================
    # ==================================================================================================================

    # Setup project paths
    pkg_project_bringup = get_package_share_directory('gz_sim_bringup')
    pkg_project_gazebo = get_package_share_directory('gz_sim_gazebo')
    pkg_project_description = get_package_share_directory('gz_sim_description')
    pkg_ros_gz_sim = get_package_share_directory('ros_gz_sim')

    # Create paths for 2 gazebo launch files: one for launching the gazebo simulation and the other for spawning models.
    gz_spawn_model_launch_path = PathJoinSubstitution(
        [pkg_ros_gz_sim, 'launch', 'gz_spawn_model.launch.py']
    )
    gz_launch_path = PathJoinSubstitution([pkg_ros_gz_sim, 'launch', 'gz_sim.launch.py'])

    """ ==============================================Important note =================================================
    The following section create elements which will be inserted in LaunchDescription to execute. The order of 
    insertion determinates the order of execution. So, [nodeA,nodeB], executes nodeA and then nodeB.
    ================================================================================================================== """

    # ================================================Converting Xacro to SDF===========================================
    # ==================================================================================================================

    # This process runs the command 'ros2 run xacro xacro path/to/world.sdf.xacro > path/to/world.sdf'.
    create_xacro_cmd = ExecuteProcess(
        cmd=[
            [
                FindExecutable(name='ros2'),
                ' run xacro xacro ',
                PathJoinSubstitution([pkg_project_gazebo, 'worlds/world.sdf.xacro']),
                '>',
                PathJoinSubstitution([pkg_project_gazebo, 'worlds/world.sdf']),
            ]
        ],
        shell=True,
    )

    # =========================================Launching Gazebo Simulation==============================================
    # ==================================================================================================================

    launch_gazebo_simulation = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(gz_launch_path),
        launch_arguments={
            'gz_args': [PathJoinSubstitution([pkg_project_gazebo, 'worlds/world.sdf -r '])],
        }.items(),
    )

    # =======================================Spawning Models in Gazebo==================================================
    # ==================================================================================================================

    # Spawns a the robot model in gazebo simulation.
    robot_position = {'x': '0.0', 'y': '0.0', 'z': '0.01', 'R': '0.0', 'P': '0.0', 'Y': '0.0'}
    mark_position = robot_position.copy()
    mark_position['x'] = str(
        float(mark_position['x']) + 0.5
    )  # spawn the mark a bit ahead of the robot

    spawn_robot_model = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(gz_spawn_model_launch_path),
        launch_arguments={
            'world': 'default',  # name of the world given in the sdf file, not the name of the sdf file itself.
            'file': [
                PathJoinSubstitution([pkg_project_description, 'models/diff_drive/model.sdf'])
            ],
            'entity_name': 'my_vehicle',
            **robot_position,
        }.items(),
    )

    # Spawns a starting point mark in gazebo simulation.
    spawn_start_point_mark_model = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(gz_spawn_model_launch_path),
        launch_arguments={
            'world': 'default',  # name of the world given in the sdf file, not the name of the sdf file itself.
            'file': [
                PathJoinSubstitution([pkg_project_description, 'models/start_point_mark/model.sdf'])
            ],
            'entity_name': 'my_mark',
            **mark_position,
        }.items(),
    )

    # ====================================Bridging and remapping Gazebo topics to ROS 2=================================
    # ==================================================================================================================

    # # # https://github.com/gazebosim/ros_gz/blob/jazzy/ros_gz_bridge/README.md
    # # # https://github.com/gazebosim/ros_gz/pull/826 to override the frame_id.
    # ros2_gazebo_bridge_node = Node(
    #     package='ros_gz_bridge',
    #     executable='parameter_bridge',
    #     arguments=[
    #         '/cmd_vel@geometry_msgs/msg/Twist]gz.msgs.Twist',
    #         '/joint_states@sensor_msgs/msg/JointState[gz.msgs.Model',
    #         '/clock@rosgraph_msgs/msg/Clock[gz.msgs.Clock',
    #         '/scan@sensor_msgs/msg/LaserScan[gz.msgs.LaserScan',
    #         '/scan/points@sensor_msgs/msg/PointCloud2[gz.msgs.PointCloudPacked',
    #         '/imu@sensor_msgs/msg/Imu[gz.msgs.IMU',
    #     ],
    #     output='screen',
    # )

    # # To make odometry and pose use the same frame used in ROS 2 another brigde is used.
    # odometry_ros2_gazebo_bridge_node = Node(
    #     package='ros_gz_bridge',
    #     executable='parameter_bridge',
    #     arguments=[
    #         '/model/my_vehicle/odometry@nav_msgs/msg/Odometry[gz.msgs.Odometry',
    #         '/model/my_vehicle/pose@geometry_msgs/msg/PoseStamped[gz.msgs.Pose',
    #     ],
    #     output='screen',
    #     parameters=[{'override_frame_id': 'odom_frame'}],
    # )

    # ==================================================================================================================

    # pose_to_odometry = Node(
    #         package='amr_simulation',
    #         executable='convert_pose_to_odometry_node',
    #         output='screen',
    #         name="convert_pose_to_odometry_node"
    # )

    # ===================================== Robot description node ====================================================
    # =================================================================================================================

    # Load the SDF file from "description" package
    sdf_file = os.path.join(pkg_project_description, 'models', 'diff_drive', 'model.sdf')
    with open(sdf_file, 'r') as infp:
        robot_desc = infp.read()

    # Takes the description and joint angles as inputs and publishes the 3D poses of the robot links
    robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        output='both',
        parameters=[
            {'use_sim_time': True},
            {'robot_description': robot_desc},
        ],
    )

    # ===================================== Rviz Node ====================================================
    # =================================================================================================================
    # Visualize in RViz
    rviz = Node(
        package='rviz2',
        executable='rviz2',
        arguments=['-d', os.path.join(pkg_project_bringup, 'config', 'diff_drive.rviz')],
        condition=IfCondition(LaunchConfiguration('rviz')),
    )

    # ===================================== Ros and gazebo bridge ====================================================
    # =================================================================================================================
    # Bridge ROS topics and Gazebo messages for establishing communication
    bridge = Node(
        package='ros_gz_bridge',
        executable='parameter_bridge',
        parameters=[
            {
                'config_file': os.path.join(pkg_project_bringup, 'config', 'gz_sim_bridge.yaml'),
                'qos_overrides./tf_static.publisher.durability': 'transient_local',
            }
        ],
        output='screen',
    )

    # ==================================================== Executing the actions ========================================
    # ===================================================================================================================

    return LaunchDescription(
        [
            DeclareLaunchArgument('rviz', default_value='true', description='Open RViz.'),
            create_xacro_cmd,
            RegisterEventHandler(
                OnProcessExit(target_action=create_xacro_cmd, on_exit=[launch_gazebo_simulation])
            ),
            RegisterEventHandler(
                OnExecutionComplete(
                    target_action=launch_gazebo_simulation,
                    on_completion=[spawn_robot_model, spawn_start_point_mark_model],
                )
            ),
            # bridge,
            # robot_state_publisher,
            # rviz,
        ]
    )
