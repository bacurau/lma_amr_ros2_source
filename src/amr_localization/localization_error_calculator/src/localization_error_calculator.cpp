#include "localization_error_calculator/localization_error_calculator.hpp"

using namespace localization_error_calculator;



LocalizationErrorCalculator::LocalizationErrorCalculator():
  rclcpp::Node("calculate_localization_error_node"){

    this->subscriber_odometry_from_ekf = this->create_subscription<nav_msgs::msg::Odometry>(
      "/odometry/filtered", rclcpp::QoS(10), std::bind(&LocalizationErrorCalculator::GetEkfOdometryMsgCallback,this, std::placeholders::_1));
    subscriber_perfect_odometry_from_gazebo = this->create_subscription<nav_msgs::msg::Odometry>(
      "/gazebo_perfect_pose", rclcpp::QoS(10), std::bind(&LocalizationErrorCalculator::GetGazeboPerfectOdometryMsgCallback
      ,this, std::placeholders::_1));
    
    
    publisher_localization_error = create_publisher<nav_msgs::msg::Odometry>("localization_error",3);

}


void LocalizationErrorCalculator::GetEkfOdometryMsgCallback(const nav_msgs::msg::Odometry::SharedPtr ekf_odometry_msg){

}

void LocalizationErrorCalculator::GetGazeboPerfectOdometryMsgCallback(const nav_msgs::msg::Odometry::SharedPtr gazebo_perfect_odometry_msg){

}



int main(int argc, char ** argv)
{
  (void) argc;
  (void) argv;

  printf("hello world localization_error_calculator_package package\n");
  return 0;
}
