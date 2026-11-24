// Documentation for the point cloud
// https://docs.ros.org/en/noetic/api/sensor_msgs/html/msg/PointCloud2.html

#include <filesystem>
#include <iostream>
#include <vector>
#include <mutex>

#include "rclcpp/rclcpp.hpp"

#include "tf2_ros/transform_broadcaster.hpp"
#include "pcl_conversions/pcl_conversions.h"
#include "sensor_msgs/msg/point_cloud2.hpp"

#include <pcl/point_types.h>
#include <pcl/point_cloud.h>

using namespace std::chrono_literals;

// void main_2();
int main_2(rclcpp::Logger logger, pcl::PointCloud<pcl::PointXYZ> &cloud_back, pcl::PointCloud<pcl::PointXYZ> &cloud_front, Eigen::Affine3d &transform);

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

    tf_broadcaster_ =
        std::make_unique<tf2_ros::TransformBroadcaster>(*this);
  }

private:
  void topic_callback(size_t idx, sensor_msgs::msg::PointCloud2::UniquePtr msg)
  {
    std::scoped_lock lock(this->storage_mutex_);
    this->point_clouds_[idx] = std::move(msg);
  };

  void timer_callback()
  {
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

      geometry_msgs::msg::TransformStamped t;

      t.header.stamp = this->get_clock()->now();
      t.header.frame_id = "cal_Sick_Back_D_Link";
      t.child_frame_id = "cal_Sick_Front_D_Link";

      Eigen::Affine3d transform;

      if (main_2(this->get_logger(), cloud_back, cloud_front, transform) != 0)
      {
        transform = Eigen::Affine3d::Identity();
      }

      auto translation = transform.translation();

      t.transform.translation.x = translation.x();
      t.transform.translation.y = translation.y();
      t.transform.translation.z = translation.z();

      auto rotation = Eigen::Quaterniond(transform.rotation());

      t.transform.rotation.x = rotation.x();
      t.transform.rotation.y = rotation.y();
      t.transform.rotation.z = rotation.z();
      t.transform.rotation.w = rotation.w();

      tf_broadcaster_->sendTransform(t);
    }
    else
    {
      RCLCPP_WARN(this->get_logger(), "Not all cloud msgs have been heard yet, skipping this iteration");
    }
  };

  std::vector<rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr>
      subscriptions_;
  std::vector<sensor_msgs::msg::PointCloud2::UniquePtr>
      point_clouds_;
  std::mutex storage_mutex_;

  rclcpp::TimerBase::SharedPtr timer_;

  std::unique_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
};

int main(int argc, char *argv[])
{
  rclcpp::NodeOptions opt;
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<PointCloudSubscriber>());
  rclcpp::shutdown();
  return 0;
}
