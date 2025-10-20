from launch import LaunchDescription
from launch.actions import (
    RegisterEventHandler,
    LogInfo,
    TimerAction
)
from launch.event_handlers import OnProcessStart
from launch_ros.actions import Node




def generate_launch_description():
    
    
    microROS_node = Node(
        package='micro_ros_agent',
        executable='micro_ros_agent',
        name='micro_ros_agent',
        output='screen',
        arguments=["serial", "--dev", "/dev/ttyACM0", "-b", "1500000"],
    )
    
    reset_arm_node = Node(
        package='amr_serial',
        executable='reset_arm',
        name='reset_arm',
        output='screen'
    )




    microROS_launch_description = LaunchDescription()
    microROS_launch_description.add_action(microROS_node)
    microROS_launch_description.add_action(reset_arm_node)
    return microROS_launch_description