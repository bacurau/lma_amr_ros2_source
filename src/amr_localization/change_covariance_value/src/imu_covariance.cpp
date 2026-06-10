#include "change_covariance_value/imu_covariance.hpp"


using namespace imu_covariance_namespace;



ImuCovariance::ImuCovariance(): rclcpp::Node("imu_covariance_calculator_node"){
  this->subscriber_imu_from_sensor = this->create_subscription<sensor_msgs::msg::Imu>(
    "/imu", rclcpp::QoS(1), std::bind(&ImuCovariance::GetImuMsgAndChangeCovarianceValues,this, std::placeholders::_1));
  this->publisher_imu_with_changed_covariance_values = create_publisher<sensor_msgs::msg::Imu>("new_imu_data",1);
  this->publiser_odom_from_imu = create_publisher<nav_msgs::msg::Odometry>("odom_from_imu",1);
}



void ImuCovariance::GetImuMsgAndChangeCovarianceValues(const sensor_msgs::msg::Imu::SharedPtr imu_msg_from_sensor ){
    CalculateImuPose(imu_msg_from_sensor);
    auto imu_msg = std::make_unique<sensor_msgs::msg::Imu>(*imu_msg_from_sensor);
    imu_msg->header.stamp = this->get_clock()->now();
    // imu_msg->orientation_covariance[0] = 1e-3;
    // imu_msg->orientation_covariance[4] = 1e-3;
    // imu_msg->orientation_covariance[8] = 1e-3;

    imu_msg->angular_velocity_covariance[0] = 1e-2;
    imu_msg->angular_velocity_covariance[4] = 1e-2;
    imu_msg->angular_velocity_covariance[8] = 0.01;

    imu_msg->linear_acceleration_covariance[0] = 1e-2;
    imu_msg->linear_acceleration_covariance[4] = 1e-2;
    imu_msg->linear_acceleration_covariance[8] = 1e-4;


    publisher_imu_with_changed_covariance_values->publish(std::move(imu_msg));
    //std::make_unique<sensor_msgs::msg::Imu>();
}

void ImuCovariance::CalculateImuPose(const sensor_msgs::msg::Imu::SharedPtr imu_msg_from_sensor){
  
  // angular velocity from imu is in radian/s  
  rclcpp::Time time = imu_msg_from_sensor->header.stamp;
  double bias = -0.05;
  current_x_linear_acceleration += ( imu_msg_from_sensor->linear_acceleration.x - current_x_linear_acceleration) * 0.1 ;
  current_yaw_rate = imu_msg_from_sensor->angular_velocity.z;
  auto duration = rclcpp::Duration::from_nanoseconds(time.nanoseconds() - last_time.nanoseconds());
  double delta_t =  duration.seconds();

  current_x_linear_acceleration = std::abs(current_x_linear_acceleration) > 0.05?current_x_linear_acceleration:0.0;
  current_yaw_rate = std::abs(current_yaw_rate)>0.010?current_yaw_rate:0.0;

  if(last_time.nanoseconds() > 0){
    current_x_linear_velocity = previous_x_linear_velocity + current_x_linear_acceleration*delta_t;
    current_x_linear_velocity = std::abs(current_x_linear_velocity)>0.0010?current_x_linear_velocity:0.0;
    current_yaw_angle =  std::fmod(current_yaw_angle + current_yaw_rate*delta_t,2.0*PI);
    double delta_s = previous_x_linear_velocity*delta_t + (current_x_linear_acceleration*delta_t*delta_t)*0.5; // s = s_i +v_i*t + (a*t^2)/2
    x_pose += -cos(current_yaw_angle)* delta_s;
    y_pose += sin(current_yaw_angle)* delta_s;

    auto odometry_msg_from_imu = std::make_unique<nav_msgs::msg::Odometry>();
    odometry_msg_from_imu->header.frame_id = "odom_frame";
    odometry_msg_from_imu->child_frame_id  = "Base_Link";
    odometry_msg_from_imu->header.stamp = time;
    odometry_msg_from_imu->pose.pose.position.x = x_pose;
    odometry_msg_from_imu->pose.pose.position.y = y_pose;
    odometry_msg_from_imu->pose.pose.position.z = 0.0;

    tf2::Quaternion q;
    q.setRPY(0.0, 0.0, current_yaw_angle);

    odometry_msg_from_imu->pose.pose.orientation.x = q.x();
    odometry_msg_from_imu->pose.pose.orientation.y = q.y();
    odometry_msg_from_imu->pose.pose.orientation.z = q.z();
    odometry_msg_from_imu->pose.pose.orientation.w = q.w();

    odometry_msg_from_imu->twist.twist.linear.x  = current_x_linear_velocity;
    odometry_msg_from_imu->twist.twist.linear.y  = previous_x_linear_velocity;
    odometry_msg_from_imu->twist.twist.angular.z = current_yaw_rate;
    odometry_msg_from_imu->twist.twist.angular.y = current_x_linear_acceleration;

    publiser_odom_from_imu->publish(std::move(odometry_msg_from_imu));
  }
  previous_x_linear_velocity = current_x_linear_velocity;
  last_time = time;
}


int main(int argc, char ** argv)
{
  (void) argc;
  (void) argv;

  rclcpp::init(argc, argv); 

  rclcpp::executors::SingleThreadedExecutor executor;

  auto imu_covariance_modifier_node = 
    std::make_shared<ImuCovariance>(); /*!< Create object of Odometry Class in node format for ROS2*/

  executor.add_node(imu_covariance_modifier_node); /*!< Add node to executor*/
  executor.spin();

  rclcpp::shutdown();
  return 0;
}
