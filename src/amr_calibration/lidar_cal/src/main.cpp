// Documentation for the point cloud
// https://docs.ros.org/en/noetic/api/sensor_msgs/html/msg/PointCloud2.html

#include "rclcpp/rclcpp.hpp"

#include "pcl_conversions/pcl_conversions.h"
#include "sensor_msgs/msg/point_cloud2.hpp"
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>

#include <filesystem>
#include <iostream>
#include <vector>

using namespace std::chrono_literals;

void main_2(void);

class PointCloudSubscriber : public rclcpp::Node
{
public:
  PointCloudSubscriber() : Node("lidar_calibration")
  {
    // TODO move this

    pcl::PointCloud<pcl::PointXYZ> out_msg;

    auto topic_callback =
        [&, this](sensor_msgs::msg::PointCloud2::UniquePtr msg) -> void
    {
      RCLCPP_INFO(this->get_logger(), "Captured point cloud");
      pcl::fromROSMsg(*(msg.get()), out_msg);
    };
    subscription_ = this->create_subscription<sensor_msgs::msg::PointCloud2>(
        "topic", 10, topic_callback);
  }

private:
  rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr subscription_;

  std::vector<rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr>
      subscriptions_;
};

int main(int argc, char *argv[])
{
  sensor_msgs::msg::PointCloud2 cloud_msg;
  auto cwd = std::filesystem::current_path();
  std::cout << "Hello world! " << std::endl;
  std::cout << "I'm in: " << cwd << std::endl;

  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<PointCloudSubscriber>());
  rclcpp::shutdown();
  return 0;
}
