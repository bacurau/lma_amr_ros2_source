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
/**
 * \brief The Odometry class
 * \details This class is responsible for calculating and publishing the robot odometry based on wheel encoder and IMU data???
 */  
class Odometry
{
 public:
  explicit Odometry(std::shared_ptr<rclcpp::Node> & nh);
  virtual ~Odometry(){};

 private:
  bool calculate_odometry(const rclcpp::Duration & duration);
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

  std::shared_ptr<rclcpp::Node> nh_;
  std::unique_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;

  rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr odom_pub_; /*!< publisher for topic "odom", with qos 5*/
  rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr corr_odom_pub_; /* !< publisher for topic "corr_motor_odom, with qos 5" */
  rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr pub_; /*!< publisher for topic "joint_states_jetson", with qos 5*/
  rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr docking_finished_pub_;/*!<publisher for topic "docking_finished", with qos 5*/
  rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr joint_state_sub_; /*!< subscriber for topic "joint_states", with qos = rclcpp::QoS(rclcpp::SensorDataQoS());*/
  rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr imu_sub_; /*!< subscriber for topic "imu", with qos = rclcpp::QoS(rclcpp::SensorDataQoS());*/


  rclcpp::Service<odometry_msgs::srv::UpdateOdometry>::SharedPtr update_odometry_server_; /*!< service server for "odometry/update_odometry"*/

  double wheels_separation_; /*!< Distance between the two wheels.*/
  double wheels_radius_left; /* Radius of left wheel.*/
  double wheels_radius_right; /* Radius of right wheel.*/
  /**   \brief the yaw angle provided by the imu in the prior call of the Odometry::calculate_odometry plus a correction. 
  *  
  *  Order of events: <br>
  *   1- Some node publishes in the "joint_states" topic. <br>
  *   2- Odometry::joint_state_callback is triggered. <br>
  *   3- Odometry::calculate_odometry is called. <br>
  *   4- last_theta is updated at the end of the Odometry::calculate_odometry function with the current imu angle.  <br>
  */
  double last_theta; 
   /** 
  \brief Used to count the first 3 calls of the Odometry::calculate_odometry function after it passes the return false condition. 
  \attention Why is this needed? 
  */
  int count_imu;
  std::string frame_id_of_odometry_; /*!< Frame id of the odometry used in the header of the Odometry message (nav_msgs::msg::Odometry).*/
  std::string child_frame_id_of_odometry_; /*!< Frame if of child_frame used in the Odometry message (nav_msgs::msg::Odometry)*/


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
  double v_x; /*!< linear velocity along the x axis.*/
  double w_z; /*!< angular velocity around the z axis.*/

  std::array<double,3> robot_pose_; /*!<Is the robot pose. <br> robot_pose_[0]: position in the x axis,<br>robot_pose_[1]: position in the y axis,<br>robot_pose_[2]: yaw angle (spin around the z axis) */
  std::array<double,3> robot_vel_; /*!<Is the robot velocity. <br> robot_vel_[0]: velocity in the x axis,<br>robot_vel_[1]: velocity in the y axis,<br>robot_vel_[2]: yaw angle velocity (spin around the z axis)*/
};
} // scooby
} // lma
#endif //scooby_NODE_ODOMETRY_HPP_