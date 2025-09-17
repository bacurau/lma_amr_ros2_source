/*******************************************************************************
* Copyright 2019 ROBOTIS CO., LTD.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

/* Author: Olmerg */

#ifndef SCOOBY_NODE_ODOMETRY_HPP_
#define SCOOBY_NODE_ODOMETRY_HPP_

#include <array>
#include <chrono>
#include <memory>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <message_filters/subscriber.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <message_filters/synchronizer.h>
#include <nav_msgs/msg/odometry.hpp>
#include <std_msgs/msg/bool.hpp>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include <tf2_ros/transform_broadcaster.h>
#include <tf2/LinearMath/Quaternion.h>
#include <geometry_msgs/msg/pose_with_covariance_stamped.hpp>

#include "odometry_msgs/srv/update_odometry.hpp"

namespace lma
{
namespace scooby
{
class Odometry
{
 public:
  explicit Odometry(std::shared_ptr<rclcpp::Node> & nh);
  virtual ~Odometry(){};

 private:
  bool calculate_odometry(const rclcpp::Duration & duration);
  void initialPoseReceived(nav_msgs::msg::Odometry::SharedPtr msg);
  void update_imu(const std::shared_ptr<sensor_msgs::msg::Imu const> & imu);
  void update_joint_state(const std::shared_ptr<sensor_msgs::msg::JointState const> & joint_state);
  void publishJointState(const rclcpp::Time & now, const std::shared_ptr<sensor_msgs::msg::JointState const> & msg);

  void joint_state_callback(const sensor_msgs::msg::JointState::SharedPtr joint_state_msg);
  void imu_callback(const sensor_msgs::msg::Imu::SharedPtr imu_msg);

  void UpdateOdometry(
    const std::shared_ptr<odometry_msgs::srv::UpdateOdometry::Request> request,
    std::shared_ptr<odometry_msgs::srv::UpdateOdometry::Response> response);

  void publish(const rclcpp::Time & now);
  void publish_corr(const rclcpp::Time & now);
  void startSlam();
  void stopSlam();

  std::shared_ptr<rclcpp::Node> nh_;
  std::unique_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;

  rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr odom_pub_;
  rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr corr_odom_pub_;
  rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr pub_;
  rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr docking_finished_pub_;
  rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr joint_state_sub_;
  rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr imu_sub_;
  rclcpp::Subscription<nav_msgs::msg::Odometry>::ConstSharedPtr initial_pose_sub_;

  rclcpp::Service<odometry_msgs::srv::UpdateOdometry>::SharedPtr update_odometry_server_;

  double wheels_separation_;
  double wheels_radius_left;
  double wheels_radius_right;
  double last_theta;
  int count_imu;
  std::string frame_id_of_odometry_;
  std::string child_frame_id_of_odometry_;

  rclcpp::Time slam_initial_time;

  bool use_imu_;
  bool publish_tf_;
  bool is_initialized;

  std::array<double, 2> diff_joint_positions_;
  double imu_angle_;
  double imu_vel_;
  rclcpp::Time imu_time_;

    rclcpp::Time last_time;
  // v = translational velocity [m/s]
  // w = rotational velocity [rad/s]
  double v_x,w_z;

  std::array<double,3> robot_pose_;
  std::array<double,3> robot_vel_;
};
} // scooby
} // lma
#endif //scooby_NODE_ODOMETRY_HPP_