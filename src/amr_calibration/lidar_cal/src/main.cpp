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
#include <mutex>

using namespace std::chrono_literals;

// void main_2();
int main_2(pcl::PointCloud<pcl::PointXYZ> &cloud_back, pcl::PointCloud<pcl::PointXYZ> &cloud_front);

class PointCloudSubscriber : public rclcpp::Node
{
public:
  PointCloudSubscriber() : Node("lidar_calibration")
  {

    // TODO move this

    pcl::PointCloud<pcl::PointXYZ> out_msg;
    {
      std::scoped_lock lock(this->storage_mutex_);
      this->point_clouds_.resize(2);
    }
    // Maybe change this to a message_filter.

    auto cloud_1_topic = "/Sick_Cloud_Back";
    auto cloud_2_topic = "/Sick_Cloud_Front";

    subscriptions_.push_back(this->create_subscription<sensor_msgs::msg::PointCloud2>(
        cloud_1_topic, 10,
        [this](sensor_msgs::msg::PointCloud2::UniquePtr msg)
        { this->topic_callback(0, std::move(msg)); }));

    subscriptions_.push_back(this->create_subscription<sensor_msgs::msg::PointCloud2>(
        cloud_2_topic, 10,
        [this](sensor_msgs::msg::PointCloud2::UniquePtr msg)
        { this->topic_callback(1, std::move(msg)); }));

    timer_ = this->create_wall_timer(1s, [this]()
                                     { this->timer_callback(); });
  }

private:
  void topic_callback(size_t idx, sensor_msgs::msg::PointCloud2::UniquePtr msg)
  {
    std::scoped_lock lock(this->storage_mutex_);
    this->point_clouds_[idx] = std::move(msg);
  };

  void timer_callback()
  {
    std::cout << "Start of timer callback" << std::endl;

    pcl::PointCloud<pcl::PointXYZ> cloud_back;
    pcl::PointCloud<pcl::PointXYZ> cloud_front;

    std::scoped_lock lock(this->storage_mutex_);
    if (this->point_clouds_[0] && this->point_clouds_[1])
    {
      pcl::fromROSMsg(*(this->point_clouds_[0].get()), cloud_back);
      pcl::fromROSMsg(*(this->point_clouds_[1].get()), cloud_front);

      // TODO rewrite this, it is scuffed. It kind of resembles a move.
      this->point_clouds_[0] = nullptr;
      this->point_clouds_[1] = nullptr;

      if (main_2(cloud_back, cloud_front) == 0)
      {
        std::cout << "main_2 returned 0" << std::endl;
        // this->timer_->cancel(); // Work as a single shot if it didn't fail, for debugging
      }
    }
    else
    {
      RCLCPP_WARN(this->get_logger(), "Not all cloud msgs have been heard yet, skipping this iteration");
    }
    std::cout << "End of timer callback" << std::endl;
  };

  std::vector<rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr>
      subscriptions_;
  std::vector<sensor_msgs::msg::PointCloud2::UniquePtr>
      point_clouds_;
  std::mutex storage_mutex_;

  rclcpp::TimerBase::SharedPtr timer_;
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
