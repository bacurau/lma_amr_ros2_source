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

#include "scooby_node/odometry.hpp"
#include <geometry_msgs/msg/pose_with_covariance_stamped.hpp>
#include "tf2/LinearMath/Matrix3x3.h"
#include <tf2/LinearMath/Quaternion.h>
//#include <geometry_msgs/msg/pose_stamped.hpp>

using namespace lma;
using namespace std::chrono_literals;
using namespace scooby;

/**
 * \brief Constructor of Odometry class
 * \details 1- Initializes member variables with default values or parameters from the ROS2 parameter server. <br>
 *          2- Creates publishers for odometry, corrected odometry, joint states, and docking status. <br>
 *          3- Creates subscriptions for IMU and joint state data. <br>
 *          4- Sets up a service to update odometry based on external requests.

 */
Odometry::Odometry(
  std::shared_ptr<rclcpp::Node> &nh)
: nh_(nh),
  use_imu_(false),
  publish_tf_(true),
  imu_angle_(0.0f)
{
 

  // Initialize robot pose and velocity

  RCLCPP_INFO(nh_->get_logger(), "Init Odometry");
  robot_pose_[0]= 0.005;//0.005;//0.000 -2.8781 0.005; 
  robot_pose_[1]= -0.045;//-0.045;//0.005 -1.8797 0.005;1.981  0.005
  robot_pose_[2]=0.0;  
 
  robot_vel_[0]=0;
  robot_vel_[2]=0;
  last_theta = 0.0;
  count_imu=0;
  

  // Create parameters for the ros2 node. How are the parameters being used?
  nh_->declare_parameter("odometry.frame_id", "odom");
  nh_->declare_parameter("odometry.child_frame_id", "Base_Link");

  nh_->declare_parameter("odometry.use_imu", false);
  nh_->declare_parameter("odometry.publish_tf", true);

  nh_->declare_parameter("wheels.separation", 0.74361);
  nh_->declare_parameter("wheels.radius_left", 0.102873);
  nh_->declare_parameter("wheels.radius_right", 0.102759);
//TODO:: Trocar valores!!
  // Fator de escala obtido a partir do erro identificado após 10 voltas para cada sentido. Erro: 18 graus. Fs = 18/3600
  //float fs = (1.00506 / 1.00211) * 1.0012;

  // Get the values of the parameters to set member variables
  
  nh_->get_parameter_or<double>("wheels.separation", wheels_separation_, 0.74361); // 0.74361 0.742188
  nh_->get_parameter_or<double>("wheels.radius_left", wheels_radius_left, 0.102873); 
  nh_->get_parameter_or<double>("wheels.radius_right", wheels_radius_right, 0.102759); 

  nh_->get_parameter_or<bool>(
    "odometry.use_imu",
    use_imu_,
    false);

  nh_->get_parameter_or<bool>(
    "odometry.publish_tf",
    publish_tf_,
    true);

  nh_->get_parameter_or<std::string>(
    "odometry.frame_id",
    frame_id_of_odometry_,
    std::string("odom"));

  nh_->get_parameter_or<std::string>(
    "odometry.child_frame_id",
    child_frame_id_of_odometry_,
    std::string("Base_Link"));


 // Create publishers for odometry, corrected odometry, joint states, and docking status.
 // These publishers use qos 5, however the variable qos is not used here.
 

  // auto qos = rclcpp::QoS(rclcpp::KeepLast(10));
  auto qos = rclcpp::QoS(rclcpp::SensorDataQoS());
  odom_pub_ = nh_->create_publisher<nav_msgs::msg::Odometry>("odom", 5);
  corr_odom_pub_ = nh_->create_publisher<nav_msgs::msg::Odometry>("corr_motor_odom", 5);
  pub_ = nh_->create_publisher<sensor_msgs::msg::JointState>("joint_states_jetson", 5);
  docking_finished_pub_ = nh_->create_publisher<std_msgs::msg::Bool>("docking_finished", 1);

  this->is_initialized = false;

  
  // Add TF2 broadcaster for odometry frame transformations???
  
  tf_broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>(nh_);

  last_time=nh_->now();
  use_imu_ = false; // why is imu not being used?
  
  // Create subscribers for imu and joint states. 
  // Nov 21: Removed else, because it was not needed. In the if and else, the
  // joint_state_sub_ was being created the same way. The if is needed to check
  // if imu subscription will be created.
  
  if (use_imu_)
  {
    imu_sub_ = nh_->create_subscription<sensor_msgs::msg::Imu>(
      "imu",
      qos,
      std::bind(&Odometry::imu_callback, this, std::placeholders::_1));
  }
  joint_state_sub_ = nh_->create_subscription<sensor_msgs::msg::JointState>(
    "joint_states",
    qos,
    std::bind(&Odometry::joint_state_callback, this, std::placeholders::_1));


// Create a service to update odometry. 
//  Who calls this server?
  update_odometry_server_ = nh_->create_service<odometry_msgs::srv::UpdateOdometry>(
    "odometry/update_odometry", 
    std::bind(&Odometry::UpdateOdometry, this, std::placeholders::_1, std::placeholders::_2));
}


/**
 * \brief Service callback to update odometry
 * \details This function is called when the update_odometry service is requested. <br>
 * 1- Updates the robot pose (x,y) based on the request message. <br>
 * 2- Creates a rotation matrix based on the orientation quarternions from the request message. <br>
 * 3- Gets euler angles from the rotation matrix and updates yaw angle for the robot. <br>
 * 4- Passes time to Odometry::publish_corr, so it can publish the corrected odometry. <br>
 * 5- Sets the response success to true and logs the response. <br>
 * 6- Publishes a docking finished message. <br>
 * \param[in] request The request message containing the new pose to update the odometry. <br>
 * \param[out] response The response message indicating success if the odometry was updated. <br>
 */
void Odometry::UpdateOdometry(
  const std::shared_ptr<odometry_msgs::srv::UpdateOdometry::Request> request,
  std::shared_ptr<odometry_msgs::srv::UpdateOdometry::Response> response)
{
  // Update robot pose based on the request message
  robot_pose_[0]=request->pose.pose.position.x; 
  robot_pose_[1]=request->pose.pose.position.y;
  // Creates a rotation matrix based on quarternions 
  tf2::Matrix3x3 mat(tf2::Quaternion(request->pose.pose.orientation.x, 
                                     request->pose.pose.orientation.y,
                                     request->pose.pose.orientation.z, 
                                     request->pose.pose.orientation.w));

  double yaw, pitch, roll;
   // Gets euler angles from the rotation matrix and update yaw angle for the robot.
  mat.getEulerYPR(yaw, pitch, roll);
  robot_pose_[2]=yaw;

  // construct odometry message and then publish it.
  publish_corr(last_time);

  response->success = true;
  RCLCPP_INFO(nh_->get_logger(), "sending back response: [%d]", response->success);

  std_msgs::msg::Bool is_docking_finished;
  is_docking_finished.data = true;
  // publish docking finished message
  docking_finished_pub_->publish(std::move(is_docking_finished));
}

/**
 * \brief "imu" topic data callback
 * \details A subscriber was created to the "imu" topic. Therefore, when a message is sent to "imu"
 *  this callback function is called to update the imu through the Odometry::update_imu function. <br>
 */
void Odometry::imu_callback(const sensor_msgs::msg::Imu::SharedPtr imu_msg)
{
  update_imu(imu_msg);
}

/**
 * \brief "joint_states" topic data callback
 * \details A subscriber was created to the "joint_states" topic.<br>
 * 1- Calculate the duration since the last callback was called or since initialization with the contructor Odometry::Odometry(). <br>
 * 2- Update the wheels joint states through the Odometry::update_joint_state function. <br>
 * 3- Calculate the odometry based on the duration since last callback through the Odometry::calculate_odometry function. <br>
 * 4- Publish the joint states through the Odometry::publishJointState function. <br>
 * 5- Publish the odometry through the Odometry::publish function. <br>
 * 6- Update the last_time member variable with the current time. <br>
 * \param[in] joint_state_msg The joint state message containing the current joint positions and velocities. <br>
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

  //4- Call Joint state publisher
  publishJointState(time,joint_state_msg);
  //5- Call Odometry publisher
  publish(time);
  last_time = time;
}


/**
 * \brief Publish wheel joint states
 * \details This functions publishes the wheel joint state based on the received joint state message (msg). <br>
 * \param[in] now The new time to be used in the header of the joint state message to be published. <br>
 * \param[in] msg The new wheel joint state message to be published. <br>
 */
void Odometry::publishJointState(
  const rclcpp::Time & now,const std::shared_ptr<sensor_msgs::msg::JointState const> & msg)
{
   auto msg2 = std::make_unique<sensor_msgs::msg::JointState>();
  msg2->header.frame_id=msg->header.frame_id;
  msg2->header.stamp =now;
  msg2->name=msg->name;
  msg2->position=msg->position;
  msg2->velocity=msg->velocity;
  msg2->effort=msg->effort;
  pub_->publish(std::move(msg2));
}

/**
 * \brief Publish corrected odometry
 * \details This function publishes the corrected odometry based on the current robot pose??? <br>
 * 1- Creates a new odometry message and fills in the header information. <br>
 * 2- Sets the robot's position and orientation in the odometry message. <br>
 * 3- Sets the robot's linear velocity in x and the angular velocity in z in the odometry message to zero. <br>
 * 4- Sets covariance values for pose and twist in the odometry message. <br>
 * 5- Creates a transform message for the odometry frame. <br>
 * 6- Publishes the corrected odometry message. <br>
 * 7- If publishing TF is enabled, sends the transform using the TF broadcaster. <br>
 * \attention This function should be merged with Odometry::publish to avoid code duplication, adding ifs and parameters to 
 * change the behavior instead of duplicating the entire function. <br>
 */
void Odometry::publish_corr(const rclcpp::Time & now)
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

  // This sets the velocity to zero
  odom_msg->twist.twist.linear.x  = 0;
  odom_msg->twist.twist.angular.z = 0;

  // TODO: Find more accurate covariance.
  odom_msg->pose.covariance[0] = 0.001;
  odom_msg->pose.covariance[7] = 0.001;
  odom_msg->pose.covariance[14] = 1.0e-9;
  odom_msg->pose.covariance[21] = 1.0e-9;
  odom_msg->pose.covariance[28] = 1.0e-9;
  odom_msg->pose.covariance[35] = 0.00172665;

  odom_msg->twist.covariance[0] = 1.0e-9;
  odom_msg->twist.covariance[7] = 1.0e-9;
  odom_msg->twist.covariance[14] = 1.0e-9;
  odom_msg->twist.covariance[21] = 1.0e-9;
  odom_msg->twist.covariance[28] = 1.0e-9;
  odom_msg->twist.covariance[35] = 0.0005;

  geometry_msgs::msg::TransformStamped odom_tf;

  odom_tf.transform.translation.x = odom_msg->pose.pose.position.x;
  odom_tf.transform.translation.y = odom_msg->pose.pose.position.y;
  odom_tf.transform.translation.z = odom_msg->pose.pose.position.z;
  odom_tf.transform.rotation      = odom_msg->pose.pose.orientation;

  odom_tf.header.frame_id = frame_id_of_odometry_;
  odom_tf.child_frame_id = child_frame_id_of_odometry_;
  odom_tf.header.stamp = now;

  // According to https://cplusplus.com/reference/utility/move/ and https://www.geeksforgeeks.org/cpp/stdmove-in-utility-in-c-move-semantics-move-constructors-and-move-assignment-operators/
  // It basically transfers the ownership of the object to the new owner, in this case, the publisher.
  // After the move, the odom_msg pointer in this function is no longer valid and should not be used.
  // Its faster than copying the object.
  corr_odom_pub_->publish(std::move(odom_msg));

 if (publish_tf_)
   tf_broadcaster_->sendTransform(odom_tf);
}
/**
 * \brief Publish odometry
 * \details This function publishes the odometry based on the current robot pose and velocity. <br>
 * This functions is very similar to Odometry::publish_corr, but it also includes the robot's linear velocities
 * in x and y, and angular velocity in z; and changes covariance values. <br>
 * \attention Odometry::publish and Odometry::publish_corr should be merged, with parameters and ifs to change the behavior. <br>
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
  odom_msg->pose.covariance[0] = 0.05;
  odom_msg->pose.covariance[7] = 0.05;
  odom_msg->pose.covariance[14] = 1.0e-9;
  odom_msg->pose.covariance[21] = 1.0e-9;
  odom_msg->pose.covariance[28] = 1.0e-9;
  odom_msg->pose.covariance[35] = 0.0872665;

  odom_msg->twist.covariance[0] = 1.0e-9;
  odom_msg->twist.covariance[7] = 1.0e-9;
  odom_msg->twist.covariance[14] = 1.0e-9;
  odom_msg->twist.covariance[21] = 1.0e-9;
  odom_msg->twist.covariance[28] = 1.0e-9;
  odom_msg->twist.covariance[35] = 0.0001;

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
 * \brief Calculate v_x, w_z, wheel joint displacement compared to previous state and update wheel joint position. <br>
 * \details 1- Calculates joint displacement since last update for both wheels. <br>
 * 2- Calculate the linear velocity for the right and left wheels.
 * 3- Calculate the linear velocity in x for the robot. <br>
 * 4- Calculate the angular velocity in z for the robot. <br>
 * 5- Updates last joint positions for both wheels. <br>
 * \param[in] joint_state The joint state message containing the current joint positions and velocities. <br>
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
 * \brief Update IMU data
 * \details This function updates the IMU angle and angular velocity based on the received IMU message. <br>
 * 1- Calculates the IMU yaw angle using the orientation quarternions from the IMU message. <br>
 * 2- Updates the IMU angular velocity in z from the IMU message. <br>
 * 3- Updates the IMU timestamp from the IMU message. <br>
 * \attention The formula used to calculate yaw from quarternions may not be correct, the formula is diffent from the standard for unit quarternions.
 * For unit quarternions, <br>
 * yaw = atan2(2.0*(x*y + w*z), 1 - 2.0*(y*y + z*z))<br>. Source https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles.
 * Other way to convert quarternions to euler angles is shown below (used at Odometry::UpdateOdometry): <br>
 * \code
 * tf2::Matrix3x3 mat(tf2::Quaternion(
 *     request->pose.pose.orientation.x,
 *     request->pose.pose.orientation.y,
 *     request->pose.pose.orientation.z,
 *     request->pose.pose.orientation.w));
 *
 * double yaw, pitch, roll;
 * // Gets euler angles from the rotation matrix and update yaw angle for the robot.
 * mat.getEulerYPR(yaw, pitch, roll);
 * robot_pose_[2] = yaw;
 * \endcode
 * 
 */
void Odometry::update_imu(const std::shared_ptr<sensor_msgs::msg::Imu const> &imu)
{
  // Gets yaw angle from quarternion, however I do not know if this formula is correct.
  // For unit quarternions, yaw = atan2(2.0*(x*y + w*z), 1 - 2.0*(y*y + z*z))
  // Source https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
  imu_angle_ = atan2f(
    imu->orientation.x*imu->orientation.y + imu->orientation.w*imu->orientation.z,
    0.5f - imu->orientation.y*imu->orientation.y - imu->orientation.z*imu->orientation.z);

  imu_vel_ =imu->angular_velocity.z;
  imu_time_=imu->header.stamp;
}

/**
 * \brief Calculate odometry based on wheel joint displacements
 * \details 
 * 1- Calculates the linear displacement (delta_s) and angular displacement (delta_theta). <br>
 * 2- Updates the robot's pose (x, y, theta). <br>
 * 3- If time step was < 0.1 and >0, recalculate Odometry::vx and Odometry::wz <br>
 * 4- Updates the robot's velocity (v_x, w_z) based on the calculated displacements and time duration. <br>
 * \param[in] duration The time duration since the last odometry calculation. <br>
 * \return Returns false if time step was 0 and true otherwise, i.e., if odometry was successfully calculated. <br>
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

  //delta_s = wheels_radius_ * (wheel_r + wheel_l) / 2.0;
  delta_s = ( wheels_radius_right * wheel_r + wheels_radius_left * wheel_l) / 2.0;

  use_imu_ = false;
  if (use_imu_)
  {
    if(count_imu>3){
    rclcpp::Time time = nh_->now();//TODO:change by joint_state
   auto duration_imu = rclcpp::Duration::from_nanoseconds(time.nanoseconds() - imu_time_.nanoseconds());
   
    theta = imu_angle_+imu_vel_*duration_imu.seconds();
    delta_theta = theta - last_theta;
    last_theta = theta;
    }else{
      delta_theta=0;
      theta=0;
      last_theta=imu_angle_;
      count_imu++;
      std::cout<<last_theta*180.0/3.141593;
    }
  }
  else
  {
    //theta = wheels_radius_ * (wheel_r - wheel_l) / wheels_separation_;
    theta = (wheels_radius_right * wheel_r - wheels_radius_left * wheel_l) / wheels_separation_;

    delta_theta = theta;
    if(delta_theta>10)
      delta_theta=0;
  }
//RCLCPP_INFO(nh_->get_logger(), "ds : %f, dtheta : %f", delta_s , delta_theta );
  // compute odometric pose
  robot_pose_[0] += delta_s * cos(robot_pose_[2] + (delta_theta / 2.0));
  robot_pose_[1] += delta_s * sin(robot_pose_[2] + (delta_theta / 2.0));
  robot_pose_[2] += delta_theta;

  // RCLCPP_DEBUG(nh_->get_logger(), "x : %f, y : %f, theta : %f", robot_pose_[0], robot_pose_[1],robot_pose_[2]);

  // compute odometric instantaneouse velocity

if (step_time<0.1 && step_time>0.0){
  v_x =(delta_s / step_time);
  w_z =(delta_theta / step_time);
}

  robot_vel_[0] = v_x;
  robot_vel_[1] = 0.0;
  robot_vel_[2] = w_z;
 
  //std::cout<<robot_vel_[0]<<","<<robot_vel_[1];
  
  //RCLCPP_INFO(nh_->get_logger(), "v_x : %f, v_z : %f",robot_vel_[0], robot_vel_[2]);

  return true;
}
