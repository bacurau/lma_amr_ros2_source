#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "nav_msgs/msg/path.hpp"
#include "marvelmind_ros2_msgs/msg/hedge_position.hpp"

class HedgehogPathNode : public rclcpp::Node
{
public:
  HedgehogPathNode()
  : Node("hedgehog_path_node")
  {
    path_pub_ = this->create_publisher<nav_msgs::msg::Path>(
      "/hedgehog_path", 10);

    hedge_sub_ = this->create_subscription<marvelmind_ros2_msgs::msg::HedgePosition>(
      "/hedgehog_pos",
      10,
      std::bind(
        &HedgehogPathNode::hedgeCallback,
        this,
        std::placeholders::_1));

    path_.header.frame_id = "marvelmind_map";
  }

private:
  void hedgeCallback(
    const marvelmind_ros2_msgs::msg::HedgePosition::SharedPtr msg)
  {
    geometry_msgs::msg::PoseStamped pose;

    pose.header.stamp = this->now();
    pose.header.frame_id = "marvelmind_map";

    pose.pose.position.x = msg->x_m;
    pose.pose.position.y = msg->y_m;
    pose.pose.position.z = msg->z_m;

    // HedgePosition gives position, not orientation.
    pose.pose.orientation.x = 0.0;
    pose.pose.orientation.y = 0.0;
    pose.pose.orientation.z = 0.0;
    pose.pose.orientation.w = 1.0;

    path_.header.stamp = pose.header.stamp;
    path_.poses.push_back(pose);

    path_pub_->publish(path_);
  }

  rclcpp::Subscription<
    marvelmind_ros2_msgs::msg::HedgePosition>::SharedPtr hedge_sub_;

  rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr path_pub_;

  nav_msgs::msg::Path path_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);

  auto node = std::make_shared<HedgehogPathNode>();

  rclcpp::spin(node);

  rclcpp::shutdown();
  return 0;
}