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

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "tf2_ros/transform_broadcaster.h"
#include "tf2/utils.hpp"
// this lib is already included by "tf2_ros/transform_broadcaster.h", however all the tutorials import
// geometry_msgs as well. Thus, to follow the same pattern we include it again.
#include "geometry_msgs/msg/transform_stamped.hpp" 
#include <memory>

#define WINDOW_TO_CHECK_IF_THE_ROBOT_IS_NOT_MOVING 4
// imu is at 400 hz and odometry is at 40 hz (microros sends joints positions every 25 ms).
// Detection of stopped movement for 0.1 seconds.
// 40*0.1 = 4. This is the window size.
namespace lma
{
namespace scooby
{
/**
 * \brief The Odometry class
 * \details This class is responsible for calculating and publishing the robot wheel odometry
 */  
class Odometry : public rclcpp::Node
{
 public:
  explicit Odometry();
  virtual ~Odometry(){};

 private:

  //================================== Methods ======================================================//
  void calculate_odometry(const rclcpp::Duration & duration, const sensor_msgs::msg::JointState::SharedPtr joint_state_msg);
  void joint_state_callback(const sensor_msgs::msg::JointState::SharedPtr joint_state_msg);
  void publish(const rclcpp::Time & now);
  void liberate_imu_bias_calculation(double diff_joint_positions_left_wheel, double diff_joint_positions_right_wheel);
  
  
  //================================== Publihers ==================================//
  rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr odom_pub_; /*!< publisher for topic "odom", with qos 5*/

  //================================== Subscribers ==================================//
  rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr joint_state_sub_; /*!< subscriber for topic "joint_states", with qos = rclcpp::QoS(rclcpp::SensorDataQoS());*/


  //================================== Variables ==================================================//
  //std::shared_ptr<rclcpp::Node> nh_; /*!< Is a shared pointer that points this class (Odometry) own object instance. This object is a ROS2 node. */
  /**  This class provides an easy way to publish coordinate frame transform information. 
   * This is being used to transform the Odometry::frame_id_of_odometry_ coordinates (odom) to Odometry::child_frame_id_of_odometry_ coordinates (base_link).
  */
  std::unique_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_; 


  double wheels_separation_; /*!< Distance between the two wheels.*/
  double wheels_radius_left; /*!< Radius of left wheel.*/
  double wheels_radius_right; /*!< Radius of right wheel.*/
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
   *  Frame id of the odometry used in the header of the Odometry message (nav_msgs::msg::Odometry).
   *  Is the odom frame.
   * */
  std::string frame_id_of_odometry_; 
  
  /**
   * Frame id of the child frame used in the Odometry message (nav_msgs::msg::Odometry).
   * Is the base_link frame.
   */
  
  std::string child_frame_id_of_odometry_;
  bool publish_tf_; /*!< Flag to enable or disable the publishing of TF transforms for odometry.*/

  std::array<double, 2> diff_joint_positions_; /*!< Saves the difference in wheel joint posistion since last update.*/
  /**
   * Is initialized with the current time when the Odometry object is created.
   * Used in 2 ways: 
   * #- Used to calculate the duration since last odometry calculations. 
   * #- A time stamp to the topic "joint_state".
   * When the "joint_states" topic callback is called, the last_time variable is used to compose the Odometry message header.
   * This variable is only updated at the end of the Odometry::joint_state_callback function. <br>
   */
  rclcpp::Time last_time;
  std::array<double,3> robot_pose_; /*!<Is the robot pose. <br> robot_pose_[0]: position in the x axis,<br>robot_pose_[1]: position in the y axis,<br>robot_pose_[2]: yaw angle (spin around the z axis) */
  std::array<double,3> robot_vel_; /*!<Is the robot velocity. <br> robot_vel_[0]: velocity in the x axis,<br>robot_vel_[1]: velocity in the y axis,<br>robot_vel_[2]: yaw angle velocity (spin around the z axis)*/
  std::array<double, 2> last_joint_positions = {0.0f, 0.0f};
};
} // scooby
} // lma
#endif //scooby_NODE_ODOMETRY_HPP_