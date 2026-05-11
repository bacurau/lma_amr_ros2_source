from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import SetEnvironmentVariable, IncludeLaunchDescription,ExecuteProcess
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution,FindExecutable
from launch_ros.substitutions import FindPackageShare


# This launch was based on the launch present at the link https://gazebosim.org/docs/harmonic/ros2_launch_gazebo/

def generate_launch_description():
    ros_gz_sim_pkg_path = get_package_share_directory('ros_gz_sim')
    amr_description_pkg_path = FindPackageShare('amr_description')  #
    amr_simulation_pkg_path = FindPackageShare('amr_simulation')  #
    gz_launch_path = PathJoinSubstitution([ros_gz_sim_pkg_path, 'launch', 'gz_sim.launch.py'])
    gz_spawn_model_launch_path = PathJoinSubstitution([ros_gz_sim_pkg_path, 'launch', 'gz_spawn_model.launch.py'])

    # this process runs the command 'ros2 run xacro xacro path/to/world.sdf.xacro > path/to/world.sdf'.
    # Used to convert the file from xacro to sdf.
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
    # needed so models and scenarios can be found using model://name in the sdf file.
    set_gz_resource_path = SetEnvironmentVariable(
            'GZ_SIM_RESOURCE_PATH',[
            PathJoinSubstitution([amr_simulation_pkg_path, 'models']),
            ':',
            PathJoinSubstitution([amr_simulation_pkg_path, 'scenarios']),
            ':',
            amr_description_pkg_path
            ]
        )
     # To launch gazebo simulation.
    launch_gazebo_simulation = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(gz_launch_path),
        launch_arguments={
            'gz_args': [PathJoinSubstitution([amr_simulation_pkg_path, 'worlds/world.sdf -r'])], 
            'on_exit_shutdown': 'True'
        }.items(),
    )

    robot_position = {'x': '2.0', 'y': '4.0', 'z': '0.01', 'roll': '0.0', 'pitch': '0.0', 'yaw': '0.0'}
    # To spawn a model in gazebo simulation.
    spawn_model_node = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(gz_spawn_model_launch_path),
        launch_arguments={
            'world':'default', # name of the world given in the sdf file, not the name of the sdf file itself.
            'file': [PathJoinSubstitution([amr_simulation_pkg_path, 'models/vehicle_blue/model.sdf'])],
            'entity_name': 'my_vehicle',
            'x': robot_position['x'],
            'y': robot_position['y'],
            'z': robot_position['z'],
            'roll': robot_position['roll'],
            'pitch': robot_position['pitch'],
            'yaw': robot_position['yaw'],
            'on_exit_shutdown': 'True'
        }.items(),
    )


     # To spawn a model in gazebo simulation.
    spawn_start_point_mark_model_node = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(gz_spawn_model_launch_path),
        launch_arguments={
            'world':'default', # name of the world given in the sdf file, not the name of the sdf file itself.
            'file': [PathJoinSubstitution([amr_simulation_pkg_path, 'models/start_point_mark/model.sdf'])],
            'entity_name': 'my_mark',
            'x': str(float(robot_position['x']) + 0.5), # spawn the mark a bit ahead of the robot
            'y': robot_position['y'],
            'z': robot_position['z'],
            'roll': robot_position['roll'],
            'pitch': robot_position['pitch'],
            'yaw': robot_position['yaw'],
            'on_exit_shutdown': 'True'
        }.items(),
    )


    # Bridging and remapping Gazebo topics to ROS 2 (replace with your own topics)
    # https://github.com/gazebosim/ros_gz/blob/jazzy/ros_gz_bridge/README.md
    ros2_gazebo_bridge_node = Node(
            package='ros_gz_bridge',
            executable='parameter_bridge',
            arguments=['/cmd_vel@geometry_msgs/msg/Twist]gz.msgs.Twist',
                       '/imu@sensor_msgs/msg/Imu[gz.msgs.IMU',
                        '/joint_states@sensor_msgs/msg/JointState[gz.msgs.Model',
                        '/clock@rosgraph_msgs/msg/Clock[gz.msgs.Clock',
                         ],
            output='screen'
        )
    
    # Launch rviz
    rviz2_node = Node(
            package='rviz2',
            executable='rviz2',
            arguments=['-d', PathJoinSubstitution([amr_simulation_pkg_path, 'rviz/vehicle.rviz'])],
            output='screen'
    )



    return LaunchDescription([
        create_xacro_cmd,
        set_gz_resource_path,
        launch_gazebo_simulation,
        spawn_model_node,
        spawn_start_point_mark_model_node,
        ros2_gazebo_bridge_node,
        rviz2_node
    ])