from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, EmitEvent, LogInfo, RegisterEventHandler
from launch.events import matches_action
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import LifecycleNode
from launch_ros.event_handlers import OnStateTransition
from launch_ros.events.lifecycle import ChangeState
from lifecycle_msgs.msg import Transition
from pathlib import Path


def generate_launch_description():
    """Launch slam_toolbox (online async) and bring its lifecycle node to active.

    Same launch for simulation and real robot; only the scan topic and the clock change:
      simulation: ros2 launch amr_slam slam_launch.py
      real robot: ros2 launch amr_slam slam_launch.py use_sim_time:=false scan_topic:=<topic>
    """
    use_sim_time = LaunchConfiguration('use_sim_time')
    scan_topic = LaunchConfiguration('scan_topic')
    slam_params_file = LaunchConfiguration('slam_params_file')

    declare_use_sim_time = DeclareLaunchArgument(
        'use_sim_time', default_value='true', description='Use the Gazebo /clock')
    declare_scan_topic = DeclareLaunchArgument(
        'scan_topic', default_value='/robot/sim/front_lidar/scan',
        description='sensor_msgs/LaserScan topic used for mapping')
    declare_params_file = DeclareLaunchArgument(
        'slam_params_file',
        default_value=str(Path(get_package_share_directory('amr_slam'), 'param', 'slam_toolbox.yaml')),
        description='slam_toolbox parameters file')

    slam_toolbox_node = LifecycleNode(
        package='slam_toolbox',
        executable='async_slam_toolbox_node',
        name='slam_toolbox',
        namespace='',
        output='screen',
        parameters=[slam_params_file, {'use_sim_time': use_sim_time, 'scan_topic': scan_topic}],
    )

    configure = EmitEvent(event=ChangeState(
        lifecycle_node_matcher=matches_action(slam_toolbox_node),
        transition_id=Transition.TRANSITION_CONFIGURE))

    activate = RegisterEventHandler(OnStateTransition(
        target_lifecycle_node=slam_toolbox_node,
        start_state='configuring',
        goal_state='inactive',
        entities=[
            LogInfo(msg='[amr_slam] slam_toolbox is activating.'),
            EmitEvent(event=ChangeState(
                lifecycle_node_matcher=matches_action(slam_toolbox_node),
                transition_id=Transition.TRANSITION_ACTIVATE)),
        ]))

    return LaunchDescription([
        declare_use_sim_time, declare_scan_topic, declare_params_file,
        slam_toolbox_node, configure, activate,
    ])
