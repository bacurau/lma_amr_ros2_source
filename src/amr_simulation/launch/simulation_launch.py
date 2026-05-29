from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import SetEnvironmentVariable, IncludeLaunchDescription,ExecuteProcess
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution,FindExecutable
from launch_ros.substitutions import FindPackageShare
from launch.event_handlers import OnProcessExit,OnExecutionComplete
from launch.actions import RegisterEventHandler

# This launch was based on the launch present at the link https://gazebosim.org/docs/harmonic/ros2_launch_gazebo/

def generate_launch_description():
    
    
    #===============================================Find packages and launch files=====================================
    #==================================================================================================================
    
    # Find paths to packages from install/share.
    ros_gz_sim_pkg_path = get_package_share_directory('ros_gz_sim')
    amr_description_pkg_path = FindPackageShare('amr_description')  #
    amr_simulation_pkg_path = FindPackageShare('amr_simulation')  #

    # Create paths for 2 gazebo launch files: one for launching the gazebo simulation and the other for spawning models.
    gz_launch_path = PathJoinSubstitution([ros_gz_sim_pkg_path, 'launch', 'gz_sim.launch.py'])
    gz_spawn_model_launch_path = PathJoinSubstitution([ros_gz_sim_pkg_path, 'launch', 'gz_spawn_model.launch.py'])
    

    ''' ==============================================Important note =================================================
        The following section create elements which will be inserted in LaunchDescription to execute. The order of 
        insertion determinates the order of execution. So, [nodeA,nodeB], executes nodeA and then nodeB.
    ================================================================================================================== '''


    #================================================Converting Xacro to SDF===========================================
    #==================================================================================================================

    # This process runs the command 'ros2 run xacro xacro path/to/world.sdf.xacro > path/to/world.sdf'.
    create_xacro_cmd =  ExecuteProcess(
            cmd=[[
                FindExecutable(name='ros2'),
                ' run xacro xacro ',
                PathJoinSubstitution([amr_simulation_pkg_path, 'worlds/world.sdf.xacro']),
                '>',
                PathJoinSubstitution([amr_simulation_pkg_path, 'worlds/world.sdf']),
            ]],
            shell=True
        )
    
    #========================================Setting Gazebo environment variables======================================
    #==================================================================================================================

    # Needed so models and scenarios can be found using model://name in the sdf file.
    set_gz_resource_path = SetEnvironmentVariable(
            'GZ_SIM_RESOURCE_PATH',[
            PathJoinSubstitution([amr_simulation_pkg_path, 'models']),
            ':',
            PathJoinSubstitution([amr_simulation_pkg_path, 'scenarios']),
            ':',
            amr_description_pkg_path
            ]
        )
    
    #=========================================Launching Gazebo Simulation==============================================
    #==================================================================================================================
    
    launch_gazebo_simulation = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(gz_launch_path),
        launch_arguments={
            'gz_args': [PathJoinSubstitution([amr_simulation_pkg_path, 'worlds/world.sdf -r '])], 
            'on_exit_shutdown': 'True'
        }.items(),
    )

    #=======================================Spawning Models in Gazebo==================================================
    #==================================================================================================================

    # Spawns a the robot model in gazebo simulation.
    robot_position = {'x': '0.0', 'y': '0.0', 'z': '0.01', 'R': '0.0', 'P': '0.0', 'Y': '0.0'}
    mark_position = robot_position.copy()
    mark_position['x'] = str(float(mark_position['x']) + 0.5) # spawn the mark a bit ahead of the robot
   
    spawn_robot_model = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(gz_spawn_model_launch_path),
        launch_arguments={
            'world':'default', # name of the world given in the sdf file, not the name of the sdf file itself.
            'file': [PathJoinSubstitution([amr_simulation_pkg_path, 'models/vehicle_blue/model.sdf'])],
            'entity_name': 'my_vehicle',
            **robot_position,
        }.items(),
    )


    # Spawns a starting point mark in gazebo simulation.
    spawn_start_point_mark_model = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(gz_spawn_model_launch_path),
        launch_arguments={
            'world':'default', # name of the world given in the sdf file, not the name of the sdf file itself.
            'file': [PathJoinSubstitution([amr_simulation_pkg_path, 'models/start_point_mark/model.sdf'])],
            'entity_name': 'my_mark',
            **mark_position,
        }.items(),
    )

    #====================================Bridging and remapping Gazebo topics to ROS 2=================================
    #==================================================================================================================

    # https://github.com/gazebosim/ros_gz/blob/jazzy/ros_gz_bridge/README.md
    # https://github.com/gazebosim/ros_gz/pull/826 to override the frame_id.
    ros2_gazebo_bridge_node = Node(
            package='ros_gz_bridge',
            executable='parameter_bridge',
            arguments=['/cmd_vel@geometry_msgs/msg/Twist]gz.msgs.Twist',
                        '/joint_states@sensor_msgs/msg/JointState[gz.msgs.Model',
                        '/clock@rosgraph_msgs/msg/Clock[gz.msgs.Clock',
                        '/scan@sensor_msgs/msg/LaserScan[gz.msgs.LaserScan',
                        '/scan/points@sensor_msgs/msg/PointCloud2[gz.msgs.PointCloudPacked',
                        '/imu@sensor_msgs/msg/Imu[gz.msgs.IMU'
                         ],
            output='screen',
        )
    
    # To make odometry and pose use the same frame used in ROS 2 another brigde is used.
    odometry_ros2_gazebo_bridge_node = Node(
            package='ros_gz_bridge',
            executable='parameter_bridge',
            arguments=['/model/my_vehicle/odometry@nav_msgs/msg/Odometry[gz.msgs.Odometry',
                        '/model/my_vehicle/pose@geometry_msgs/msg/PoseStamped[gz.msgs.Pose'],
            output='screen',
            parameters=[
                {'override_frame_id': 'odom_frame'}
            ],
    )
    
    #==================================================================================================================


    pose_to_odometry = Node(
            package='amr_simulation',
            executable='convert_pose_to_odometry_node',
            output='screen',
            name="convert_pose_to_odometry_node"
    )







    #==================================================== Executing the actions ========================================
    #===================================================================================================================
    
    return LaunchDescription([
        create_xacro_cmd,
        set_gz_resource_path,
        RegisterEventHandler(
            OnProcessExit(
                target_action=create_xacro_cmd,
                on_exit=[
                    launch_gazebo_simulation
                    ]
            )
        ),
        RegisterEventHandler(
            OnExecutionComplete(
                target_action=launch_gazebo_simulation,
                on_completion=[  spawn_robot_model,
                                spawn_start_point_mark_model]
            )
        ),
        ros2_gazebo_bridge_node,
        odometry_ros2_gazebo_bridge_node,
        pose_to_odometry
    ])