#include "marvelmind_msg_processing/marvelmind_adapter.hpp"


using namespace lma;
using namespace scooby;


MarvelmindAdapter::MarvelmindAdapter(): 
  rclcpp::Node("marvelmind_adapter_node", rclcpp::NodeOptions().use_intra_process_comms(true))
{
  RCLCPP_INFO(this->get_logger(), "Init marvelmind Adapter");
 
 // Create publishers for odometry, corrected odometry, joint states, and docking status.
 // These publishers use qos 5, however the variable qos is not used here.
  auto qos = rclcpp::QoS(rclcpp::SensorDataQoS());
  
  // Add TF2 broadcaster for odometry frame transformations
  tf_broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>(this);

  this->hedge_pos_ang_subscriber = this->create_subscription<marvelmind_ros2_msgs::msg::HedgePositionAngle>(
    "/hedgehog_pos_ang", rclcpp::SensorDataQoS(), std::bind(&MarvelmindAdapter::Marvelmind_Pose_Subscription_CallBack,this, std::placeholders::_1));

  this->marvelmind_pose_publisher = this->create_publisher<geometry_msgs::msg::PoseWithCovarianceStamped>("marvelmind_pose",rclcpp::SensorDataQoS());
}

void MarvelmindAdapter::Marvelmind_Pose_Subscription_CallBack(marvelmind_ros2_msgs::msg::HedgePositionAngle::SharedPtr pose_msg_from_marvelmind){


  //RCLCPP_INFO(this->get_logger(), "Calling Callback from marvelmind");
  auto marvelming_msg = std::make_unique<geometry_msgs::msg::PoseWithCovarianceStamped>();

  marvelming_msg->header.frame_id = "map";
  marvelming_msg->header.stamp = this->get_clock()->now();

  marvelming_msg->pose.pose.position.x = pose_msg_from_marvelmind->x_m;
  marvelming_msg->pose.pose.position.y = pose_msg_from_marvelmind->y_m;
  marvelming_msg->pose.pose.position.z = 0;

  tf2::Quaternion q;
  q.setRPY(0.0, 0.0, pose_msg_from_marvelmind->angle);

  marvelming_msg->pose.pose.orientation.x = q.x();
  marvelming_msg->pose.pose.orientation.y = q.y();
  marvelming_msg->pose.pose.orientation.z = q.z();
  marvelming_msg->pose.pose.orientation.w = q.w();

  geometry_msgs::msg::TransformStamped marvelmind_tf;

  marvelmind_tf.transform.translation.x = marvelming_msg->pose.pose.position.x;
  marvelmind_tf.transform.translation.y = marvelming_msg->pose.pose.position.y;
  marvelmind_tf.transform.translation.z = marvelming_msg->pose.pose.position.z;
  marvelmind_tf.transform.rotation      = marvelming_msg->pose.pose.orientation;

  marvelmind_tf.header.frame_id = "map";
  marvelmind_tf.child_frame_id = "odom_frame";
  marvelmind_tf.header.stamp =  marvelming_msg->header.stamp;

  marvelmind_pose_publisher->publish(std::move(marvelming_msg));
  tf_broadcaster_->sendTransform(marvelmind_tf);
}