from pathlib import Path

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    """Launch wheel odometry (without its TF), IMU preprocessing and the EKF.

    The EKF publishes odom_frame -> Base_Link, so the odometry node must not. Set
    start_odometry:=false only if odometry already runs with odometry.publish_tf:=false.
    """
    use_sim_time = LaunchConfiguration('use_sim_time')
    share = get_package_share_directory('amr_ekf')
    odometry_params = Path(get_package_share_directory('odometry'), 'param', 'snoopy.yaml')

    return LaunchDescription([
        DeclareLaunchArgument('use_sim_time', default_value='false',
                              description='Use /clock (true for bag replay)'),
        DeclareLaunchArgument('start_odometry', default_value='true',
                              description='Start the wheel odometry node'),
        Node(
            package='odometry', executable='odometry_node', name='odometry_node', output='screen',
            condition=IfCondition(LaunchConfiguration('start_odometry')),
            parameters=[str(odometry_params),
                        {'use_sim_time': use_sim_time, 'odometry.publish_tf': False}]),
        Node(
            package='amr_ekf', executable='imu_fix_node', name='imu_fix', output='screen',
            parameters=[str(Path(share, 'config', 'imu_fix.yaml')),
                        {'use_sim_time': use_sim_time}]),
        Node(
            package='robot_localization', executable='ekf_node', name='ekf_filter_node',
            output='screen',
            parameters=[str(Path(share, 'config', 'ekf.yaml')), {'use_sim_time': use_sim_time}]),
    ])
