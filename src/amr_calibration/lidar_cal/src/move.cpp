#include <functional>
#include <memory>
#include <sstream>
#include <string>

#include "geometry_msgs/msg/transform_stamped.hpp"
#include "rclcpp/rclcpp.hpp"
#include "tf2/LinearMath/Quaternion.hpp"
#include "tf2_ros/transform_broadcaster.hpp"
#include "turtlesim/msg/pose.hpp"

class FramePublisher : public rclcpp::Node
{
public:
  FramePublisher()
      : Node("turtle_tf2_frame_publisher")
  {
    tf_broadcaster_ =
        std::make_unique<tf2_ros::TransformBroadcaster>(*this);

    auto handle_turtle_pose = [this](double x, double y, double z, double q_x, double q_y, double q_z, double q_w)
    {
      geometry_msgs::msg::TransformStamped t;

      t.header.stamp = this->get_clock()->now();
      t.header.frame_id = "world";
      // t.child_frame_id =

      t.transform.translation.x = x;
      t.transform.translation.y = y;
      t.transform.translation.z = z;

      // tf2::Quaternion q;
      // q.setRPY(0, 0, msg->theta);
      t.transform.rotation.x = q_x;
      t.transform.rotation.y = q_y;
      t.transform.rotation.z = q_z;
      t.transform.rotation.w = q_w;

      // Send the transformation
      tf_broadcaster_->sendTransform(t);
    };
  }

private:
  std::unique_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
};

int main(int argc, char *argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<FramePublisher>());
  rclcpp::shutdown();
  return 0;
}