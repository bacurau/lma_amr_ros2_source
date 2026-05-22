#include "convert_pose_to_odometry/convert_pose_to_odometry.hpp"

using namespace convert_pose_to_odometry;

ConvertPoseToOdometry::ConvertPoseToOdometry()
: rclcpp::Node("convert_pose_to_odometry_node"){

  this->subscriber_pose_from_gazebo = this->create_subscription<geometry_msgs::msg::PoseStamped>(
    "/model/my_vehicle/pose", rclcpp::QoS(10), // queue size
    std::bind(&ConvertPoseToOdometry::gazeboPoseCallback, this, std::placeholders::_1));
  publisher_perfect_odometry_from_gazebo_ = this->create_publisher<nav_msgs::msg::Odometry>("gazebo_perfect_pose", 3); // queue size
}

void ConvertPoseToOdometry::gazeboPoseCallback(const geometry_msgs::msg::PoseStamped::SharedPtr pose_msg)
{

  auto pose_from_gazebo_to_odom_msg = std::make_unique<nav_msgs::msg::Odometry>();


  pose_from_gazebo_to_odom_msg->header.frame_id =  pose_msg->header.frame_id;
  pose_from_gazebo_to_odom_msg->child_frame_id  = "chassis";
  pose_from_gazebo_to_odom_msg->header.stamp = pose_msg->header.stamp;

  pose_from_gazebo_to_odom_msg->pose.pose.position.x = pose_msg->pose.position.x;
  pose_from_gazebo_to_odom_msg->pose.pose.position.y = pose_msg->pose.position.y;
  pose_from_gazebo_to_odom_msg->pose.pose.position.z = pose_msg->pose.position.z;

  pose_from_gazebo_to_odom_msg->pose.pose.orientation.x = pose_msg->pose.orientation.x;
  pose_from_gazebo_to_odom_msg->pose.pose.orientation.y = pose_msg->pose.orientation.y;
  pose_from_gazebo_to_odom_msg->pose.pose.orientation.z = pose_msg->pose.orientation.z;
  pose_from_gazebo_to_odom_msg->pose.pose.orientation.w = pose_msg->pose.orientation.w;

  pose_from_gazebo_to_odom_msg->twist.twist.linear.x  = 0;
  pose_from_gazebo_to_odom_msg->twist.twist.linear.y  = 0;
  pose_from_gazebo_to_odom_msg->twist.twist.angular.z = 0;

  // TODO: Find more accurate covariance.
  // odom_msg->pose.covariance[0] = 0.25;
  // odom_msg->pose.covariance[7] = 0.25;
  // odom_msg->pose.covariance[14] = 1.0e-9;
  // odom_msg->pose.covariance[21] = 1.0e-9;
  // odom_msg->pose.covariance[28] = 1.0e-9;
  // odom_msg->pose.covariance[35] = 0.0872665;

  // odom_msg->twist.covariance[0] = 1.0e-9;
  // odom_msg->twist.covariance[7] = 1.0e-9;
  // odom_msg->twist.covariance[14] = 1.0e-9;
  // odom_msg->twist.covariance[21] = 1.0e-9;
  // odom_msg->twist.covariance[28] = 1.0e-9;
  // odom_msg->twist.covariance[35] = 0.0001;

 

  publisher_perfect_odometry_from_gazebo_->publish(std::move(pose_from_gazebo_to_odom_msg));
}






int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  rclcpp::spin(std::make_unique<convert_pose_to_odometry::ConvertPoseToOdometry>());

  rclcpp::shutdown();
  return 0;
}
