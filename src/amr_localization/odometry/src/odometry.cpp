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

#include "odometry.hpp"


using namespace lma;
using namespace std::chrono_literals;
using namespace scooby;

/**
 * \brief Constructor of Odometry class which is a ros2 node.
 * \details -# Initializes member variables with default values or parameters from the ROS2 parameter server. 
 *  -# Creates publishers for odometry, corrected odometry, joint states.
 *  -# Creates subscription joint state data.
 *  -# Sets up a service to update odometry based on external requests.

 */
Odometry::Odometry(): 
  Node("snoopy_odom", rclcpp::NodeOptions().use_intra_process_comms(true)),
  publish_tf_(true)
{

  // Initialize robot pose and velocity
  RCLCPP_INFO(this->get_logger(), "Init Odometry");
  robot_pose_[0]= 0;//0.005;//0.005;//0.000 -2.8781 0.005; 
  robot_pose_[1]= 0;//-0.045;//-0.045;//0.005 -1.8797 0.005;1.981  0.005
  robot_pose_[2]=0.0;  
 
  robot_vel_[0]=0;
  robot_vel_[2]=0;
  last_theta = 0.0;
  

  // Create parameters for the ros2 node.
  this->declare_parameter("odometry.frame_id", "odom");
  this->declare_parameter("odometry.child_frame_id", "Base_Link");
  this->declare_parameter("odometry.publish_tf", true);
  this->declare_parameter("wheels.separation", 0.74361);
  this->declare_parameter("wheels.radius_left", 0.102873);
  this->declare_parameter("wheels.radius_right", 0.102759);
//TODO:: Trocar valores!!
  // Fator de escala obtido a partir do erro identificado após 10 voltas para cada sentido. Erro: 18 graus. Fs = 18/3600
  //float fs = (1.00506 / 1.00211) * 1.0012;

  // Get the values of the parameters to set member variables
  this->get_parameter_or<double>("wheels.separation", wheels_separation_, 0.74361); // 0.74361 0.742188
  this->get_parameter_or<double>("wheels.radius_left", wheels_radius_left, 0.102873); 
  this->get_parameter_or<double>("wheels.radius_right", wheels_radius_right, 0.102759); 

  this->get_parameter_or<bool>(
    "odometry.publish_tf",
    publish_tf_,
    true);

  this->get_parameter_or<std::string>(
    "odometry.frame_id",
    frame_id_of_odometry_,
    std::string("odom"));

  this->get_parameter_or<std::string>(
    "odometry.child_frame_id",
    child_frame_id_of_odometry_,
    std::string("Base_Link"));


 // Create publishers for odometry, corrected odometry, joint states, and docking status.
 // These publishers use qos 5, however the variable qos is not used here.
  auto qos = rclcpp::QoS(rclcpp::SensorDataQoS());
  odom_pub_ = this->create_publisher<nav_msgs::msg::Odometry>("odom", 5);
  
  // Add TF2 broadcaster for odometry frame transformations???
  tf_broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>(this);

  last_time=this->now();
  // Create subscriber for wheel joint state data. 
  joint_state_sub_ = this->create_subscription<sensor_msgs::msg::JointState>(
    "joint_states",
    qos,
    std::bind(&Odometry::joint_state_callback, this, std::placeholders::_1));
}


/**
 * \brief "joint_states" topic data callback
 * \details A subscriber was created to the "joint_states" topic.
 *-# Calculate the duration since the last callback was called or since initialization with the contructor Odometry::Odometry(). 
 *-# Update the wheels joint states through the Odometry::update_joint_state function. 
 *-# Calculate the odometry based on the duration since last callback through the Odometry::calculate_odometry function.  
 *-# Publish the odometry through the Odometry::publish function. 
 *-# Update the last_time member variable with the current time. 
 * \param[in] joint_state_msg The joint state message containing the current joint positions and velocities. 
 */
void Odometry::joint_state_callback(const sensor_msgs::msg::JointState::SharedPtr joint_state_msg)
{

 // 1- Calculate the duration since the last callback was called or since initialization with the contructor Odometry::Odometry().
  rclcpp::Time time = joint_state_msg->header.stamp;
  auto duration = rclcpp::Duration::from_nanoseconds(time.nanoseconds() - last_time.nanoseconds());
  
  // 2- Update the wheels joint states.
  update_joint_state(joint_state_msg);

  // 3- Calculate the odometry based on the duration since last callback.
  calculate_odometry(duration);

  //4- Call Odometry publisher
  publish(time);
  last_time = time;
}

/**
 * \brief This function publishes the odometry based on the current robot pose and velocity. 
 * \details -# Packages the odometry message (nav_msgs::msg::Odometry).
 * -# Packages the tf2 transform message (geometry_msgs::msg::TransformStamped).
 * -# Publishes the odometry message to the "odom" topic.
 * -# Publishes the tf2 transform if Odometry::publish_tf_ is true.
 * \param[in] now is the time stamp present in the joint state message (joint_state_msg->header.stamp).
 */
void Odometry::publish(const rclcpp::Time & now)
{
  auto odom_msg = std::make_unique<nav_msgs::msg::Odometry>();

  odom_msg->header.frame_id = frame_id_of_odometry_;
  odom_msg->child_frame_id  = child_frame_id_of_odometry_;
  odom_msg->header.stamp = now;

  odom_msg->pose.pose.position.x = robot_pose_[0];
  odom_msg->pose.pose.position.y = robot_pose_[1];
  odom_msg->pose.pose.position.z = 0;

  tf2::Quaternion q;
  q.setRPY(0.0, 0.0, robot_pose_[2]);

  odom_msg->pose.pose.orientation.x = q.x();
  odom_msg->pose.pose.orientation.y = q.y();
  odom_msg->pose.pose.orientation.z = q.z();
  odom_msg->pose.pose.orientation.w = q.w();

  odom_msg->twist.twist.linear.x  = robot_vel_[0];
  odom_msg->twist.twist.linear.y  = robot_vel_[1];
  odom_msg->twist.twist.angular.z = robot_vel_[2];

  // TODO: Find more accurate covariance.
  // odom_msg->pose.covariance[0] = 0.05;
  // odom_msg->pose.covariance[7] = 0.05;
  // odom_msg->pose.covariance[14] = 1.0e-9;
  // odom_msg->pose.covariance[21] = 1.0e-9;
  // odom_msg->pose.covariance[28] = 1.0e-9;
  // odom_msg->pose.covariance[35] = 0.05;

  odom_msg->twist.covariance[0] = 0.05;
  odom_msg->twist.covariance[7] = 1.0e-9;
  odom_msg->twist.covariance[14] = 1.0e-9;
  odom_msg->twist.covariance[21] = 1.0e-9;
  odom_msg->twist.covariance[28] = 1.0e-9;
  odom_msg->twist.covariance[35] = 0.05;

  geometry_msgs::msg::TransformStamped odom_tf;

  odom_tf.transform.translation.x = odom_msg->pose.pose.position.x;
  odom_tf.transform.translation.y = odom_msg->pose.pose.position.y;
  odom_tf.transform.translation.z = odom_msg->pose.pose.position.z;
  odom_tf.transform.rotation      = odom_msg->pose.pose.orientation;

  odom_tf.header.frame_id = frame_id_of_odometry_;
  odom_tf.child_frame_id = child_frame_id_of_odometry_;
  odom_tf.header.stamp = now;

  odom_pub_->publish(std::move(odom_msg));

 if (publish_tf_)
   tf_broadcaster_->sendTransform(odom_tf);
}

/**
 * \brief Calculate v_x, w_z, wheel joint displacement compared to previous state and update wheel joint position.
 * \details-# Calculates joint displacement since last update for both wheels.
 *-# Calculate the linear velocity for the right and left wheels.
 *-# Calculate the linear velocity in x for the robot.
 *-# Calculate the angular velocity in z for the robot. 
 *-# Updates last joint positions for both wheels.
 * \param[in] joint_state The joint state message containing the current joint positions and velocities. 
 */
void Odometry::update_joint_state(
  const std::shared_ptr<sensor_msgs::msg::JointState const> &joint_state)
{
  static std::array<double, 2> last_joint_positions = {0.0f, 0.0f};
  // 1- Calculate joint displacement since last update for both wheels.
  diff_joint_positions_[0] = joint_state->position[0] - last_joint_positions[0];
  diff_joint_positions_[1] = joint_state->position[1] - last_joint_positions[1];

  // 2- Calculate the linear velocity for the right and left wheels.
  double linear_velocity_for_right_wheel = wheels_radius_right * joint_state->velocity[1]; // Vr = w_r * radius of right wheel
  double linear_velocity_for_left_wheel = wheels_radius_left * joint_state->velocity[0]; // Vl = w_l * radius of left wheel
  
  // 3- Calculate the linear velocity in x for the robot.
  v_x = (linear_velocity_for_left_wheel + linear_velocity_for_right_wheel)/2; // Robot linear velocity in x.
  //TODO:change direction for real robot

  // 4- Calculate the angular velocity in z for the robot.
  // w_z = Vr - Vl / wheels_separation
  w_z = (linear_velocity_for_right_wheel-linear_velocity_for_left_wheel)/(wheels_separation_);

  // 5- Updates last joint positions for both wheels.'
  last_joint_positions[0] = joint_state->position[0];
  last_joint_positions[1] = joint_state->position[1];
}

/**
 * \brief Calculate odometry based on wheel joint displacements
 * \details 
 *-# Calculates the linear displacement (delta_s) and angular displacement (delta_theta).
 *-# Updates the robot's pose (x, y, theta).
 *-# If time step was < 0.1 and >0, recalculate Odometry::vx and Odometry::wz
 *-# Updates the robot's velocity (Odometry::robot_vel_)
 * \param[in] duration The time duration since the last odometry calculation.
 * \return Returns false if time step was 0 and true otherwise, i.e., if odometry was successfully calculated.
 */
bool Odometry::calculate_odometry(const rclcpp::Duration &duration)
{

  // rotation value of wheel [rad]
  //TODO: change for real robot
  double wheel_l = diff_joint_positions_[0];
  double wheel_r = diff_joint_positions_[1];
  double delta_s = 0.0;
  double delta_theta = 0.0;
  double theta = 0.0;
  double step_time = duration.seconds();

  if (step_time == 0.0)
    return false;

  if (std::isnan(wheel_l))
    wheel_l = 0.0;

  if (std::isnan(wheel_r))
    wheel_r = 0.0;

  delta_s = ( wheels_radius_right * wheel_r + wheels_radius_left * wheel_l) / 2.0;
  theta = (wheels_radius_right * wheel_r - wheels_radius_left * wheel_l) / wheels_separation_;

  delta_theta = theta;
  if(delta_theta>10){
    delta_theta=0;
  }

  robot_pose_[0] += delta_s * cos(robot_pose_[2] + (delta_theta / 2.0));
  robot_pose_[1] += delta_s * sin(robot_pose_[2] + (delta_theta / 2.0));
  robot_pose_[2] += delta_theta;

 
  // compute odometric instantaneouse velocity
  if (step_time<0.1 && step_time>0.0){
    v_x =(delta_s / step_time);
    w_z =(delta_theta / step_time);
  }

  robot_vel_[0] = v_x;
  robot_vel_[1] = 0.0;
  robot_vel_[2] = w_z;

  return true;
}