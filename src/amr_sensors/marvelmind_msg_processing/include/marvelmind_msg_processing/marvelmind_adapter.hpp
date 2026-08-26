#ifndef MARVELMIND_ADAPTER_HPP
#define MARVELMIND_ADAPTER_HPP


#include <rclcpp/rclcpp.hpp>
#include "geometry_msgs/msg/pose_with_covariance_stamped.hpp"
#include "marvelmind_ros2_msgs/msg/hedge_position.hpp"
#include "marvelmind_ros2_msgs/msg/hedge_position_angle.hpp"
#include "tf2/utils.hpp"
#include "tf2_ros/transform_broadcaster.h"


namespace lma
{
namespace scooby
{
/**
 * \brief The Odometry class
 * \details This class is responsible for calculating and publishing the robot wheel odometry
 */  
class MarvelmindAdapter : public rclcpp::Node
{
 public:
  explicit MarvelmindAdapter();
  virtual ~MarvelmindAdapter(){};

 private:

  //================================== Methods ======================================================//
  void Marvelmind_Pose_Subscription_CallBack(marvelmind_ros2_msgs::msg::HedgePositionAngle::SharedPtr pose_msg_from_marvelmind);
  
  
  //================================== Publihers ==================================//
  rclcpp::Publisher<geometry_msgs::msg::PoseWithCovarianceStamped>::SharedPtr marvelmind_pose_publisher; /*!< publisher for topic "marvelmind_pose"*/

  //================================== Subscribers ==================================//
  rclcpp::Subscription<marvelmind_ros2_msgs::msg::HedgePositionAngle>::SharedPtr hedge_pos_ang_subscriber;

  //================================== Variables ==================================================//
  std::unique_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_; 

};
} // scooby
} // lma




#endif