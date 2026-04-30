| Number | Topic    | Type          | Publisher(s)           | Subscriber(s)            | Details                |
|:-----------------:|:-----------------------------:|:-------------:|:------------------------:|:--------------------------:|:----------------------:|
|1|/Sick_Cloud_Back|sensor_msgs/msg/PointCloud2|/sick_scan_back_node|/sick_scan_back_node, /sick_scan_back_node||
|2|/Sick_Cloud_Front|sensor_msgs/msg/PointCloud2|/sick_scan_front_node|/sick_scan_front_node, /sick_scan_front_node||
|3|/battery_state|sensor_msgs/msg/BatteryState|/acquisition_system|||
|4|/clock|rosgraph_msgs/msg/Clock|/acquisition_system|||
|5|/cmd_vel|geometry_msgs/msg/Twist||/acquisition_system||
|6|/diagnostic_array|diagnostic_msgs/msg/DiagnosticArray|/acquisition_system|||
|7|/diagnostics|diagnostic_msgs/msg/DiagnosticArray|/sick_scan_back_node, /sick_scan_front_node|||
|8|/fake_imu_sick|sensor_msgs/msg/Imu|/sick_scan_back_node, /sick_scan_front_node|||
|9|/filter/euler|geometry_msgs/msg/Vector3Stamped|/xsens_mti_node|||
|10|/filter/free_acceleration|geometry_msgs/msg/Vector3Stamped|/xsens_mti_node|||
|11|/filter/positionlla|geometry_msgs/msg/Vector3Stamped|/xsens_mti_node|||
|12|/filter/quaternion|geometry_msgs/msg/QuaternionStamped|/xsens_mti_node|||
|13|/filter/twist|geometry_msgs/msg/TwistStamped|/xsens_mti_node|||
|14|/filter/velocity|geometry_msgs/msg/Vector3Stamped|/xsens_mti_node|||
|15|/gnss|sensor_msgs/msg/NavSatFix|/xsens_mti_node|||
|16|/gnss_pose|geometry_msgs/msg/PoseStamped|/xsens_mti_node|||
|17|/imu|sensor_msgs/msg/Imu|/acquisition_system|||
|18|/imu/acceleration|geometry_msgs/msg/Vector3Stamped|/xsens_mti_node|||
|19|/imu/angular_velocity|geometry_msgs/msg/Vector3Stamped|/xsens_mti_node|||
|20|/imu/data|sensor_msgs/msg/Imu|/xsens_mti_node|||
|21|/imu/mag|sensor_msgs/msg/MagneticField|/xsens_mti_node|||
|22|/imu/time_ref|sensor_msgs/msg/TimeReference|/xsens_mti_node|||
|23|/imu/utctime|sensor_msgs/msg/TimeReference|/xsens_mti_node|||
|24|/joint_states|sensor_msgs/msg/JointState|/acquisition_system|/robot_state_publisher_node||
|25|/nmea|nmea_msgs/msg/Sentence|/xsens_mti_node|||
|26|/odometry_ARM|nav_msgs/msg/Odometry|/acquisition_system|||
|27|/pressure|sensor_msgs/msg/FluidPressure|/xsens_mti_node|||
|28|/range_back|sensor_msgs/msg/Range|/acquisition_system|||
|29|/range_back_left|sensor_msgs/msg/Range|/acquisition_system|||
|30|/range_back_right|sensor_msgs/msg/Range|/acquisition_system|||
|31|/range_front|sensor_msgs/msg/Range|/acquisition_system|||
|32|/range_front_left|sensor_msgs/msg/Range|/acquisition_system|||
|33|/range_front_right|sensor_msgs/msg/Range|/acquisition_system|||
|34|/range_left|sensor_msgs/msg/Range|/acquisition_system|||
|35|/range_left_left|sensor_msgs/msg/Range|/acquisition_system|||
|36|/range_left_right|sensor_msgs/msg/Range|/acquisition_system|||
|37|/range_right|sensor_msgs/msg/Range|/acquisition_system|||
|38|/range_right_left|sensor_msgs/msg/Range|/acquisition_system|||
|39|/range_right_right|sensor_msgs/msg/Range|/acquisition_system|||
|40|/robot_description|std_msgs/msg/String|/robot_state_publisher_node|||
|41|/rtcm|mavros_msgs/msg/RTCM||/xsens_mti_node||
|42|/sick_tim_5xx_back/encoder|sick_scan_xd/msg/Encoder|/sick_scan_back_node|||
|43|/sick_tim_5xx_back/scan|sensor_msgs/msg/LaserScan|/sick_scan_back_node|||
|44|/sick_tim_5xx_front/encoder|sick_scan_xd/msg/Encoder|/sick_scan_front_node|||
|45|/sick_tim_5xx_front/scan|sensor_msgs/msg/LaserScan|/sick_scan_front_node|||
|46|/status|xsens_mti_ros2_driver/msg/XsStatusWord|/xsens_mti_node|||
|47|/temperature|sensor_msgs/msg/Temperature|/xsens_mti_node|||
|48|/temperature_air|sensor_msgs/msg/Temperature|/acquisition_system|||
|49|/temperature_battery|sensor_msgs/msg/Temperature|/acquisition_system|||
|50|/temperature_left_motor|sensor_msgs/msg/Temperature|/acquisition_system|||
|51|/temperature_right_motor|sensor_msgs/msg/Temperature|/acquisition_system|||
|52|/tf|tf2_msgs/msg/TFMessage|/robot_state_publisher_node|||
|53|/tf_static|tf2_msgs/msg/TFMessage|/robot_state_publisher_node|||
