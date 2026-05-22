#ifndef CONVERT_POSE_TO_ODOMETRY
#define CONVERT_POSE_TO_ODOMETRY

#include  "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include <string>



namespace convert_pose_to_odometry
{

class ConvertPoseToOdometry : public rclcpp::Node
{
public:
  explicit ConvertPoseToOdometry();
  ~ConvertPoseToOdometry(){};
  // ~ConvertJoystickCommandsToCmdVel();

private:
  
  void gazeboPoseCallback(const geometry_msgs::msg::PoseStamped::SharedPtr pose_msg);
  rclcpp::Subscription<geometry_msgs::msg::PoseStamped>::SharedPtr subscriber_pose_from_gazebo;
  rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr publisher_perfect_odometry_from_gazebo_;
  std::string frame_id_of_odometry_ = "odom_frame";
};


}  // namespace convert_pose_to_odometry

#endif  // CONVERT_POSE_TO_ODOMETRY_HPP_
