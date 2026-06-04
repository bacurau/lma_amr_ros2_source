#include "change_covariance_value/imu_covariance.hpp"


using namespace imu_covariance_namespace;


ImuCovariance::ImuCovariance(): rclcpp::Node("imu_covariance_calculator_node"){
  this->subscriber_imu_from_sensor = this->create_subscription<sensor_msgs::msg::Imu>(
    "/imu/data", rclcpp::QoS(10), std::bind(&ImuCovariance::GetImuMsgAndChangeCovarianceValues,this, std::placeholders::_1));
  this->publisher_imu_with_changed_covariance_values = create_publisher<sensor_msgs::msg::Imu>("new_imu_data",3);
}



void ImuCovariance::GetImuMsgAndChangeCovarianceValues(const sensor_msgs::msg::Imu::SharedPtr imu_msg_from_sensor ){
    auto imu_msg = std::make_unique<sensor_msgs::msg::Imu>(*imu_msg_from_sensor);
    imu_msg->header.stamp = this->get_clock()->now();
    // imu_msg->orientation_covariance[0] = 1e-3;
    // imu_msg->orientation_covariance[4] = 1e-3;
    // imu_msg->orientation_covariance[8] = 1e-3;

    imu_msg->angular_velocity_covariance[0] = 1e-2;
    imu_msg->angular_velocity_covariance[4] = 1e-2;
    imu_msg->angular_velocity_covariance[8] = 1e-2;

    imu_msg->linear_acceleration_covariance[0] = 1e-2;
    imu_msg->linear_acceleration_covariance[4] = 1e-2;
    imu_msg->linear_acceleration_covariance[8] = 1e-4;


    publisher_imu_with_changed_covariance_values->publish(std::move(imu_msg));
    //std::make_unique<sensor_msgs::msg::Imu>();
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
