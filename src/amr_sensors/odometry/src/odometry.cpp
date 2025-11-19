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
 

  /**
   * Initialize robot pose and velocity
   */
  RCLCPP_INFO(nh_->get_logger(), "Init Odometry");
  robot_pose_[0]= 0.005;//0.005;//0.000 -2.8781 0.005; 
  robot_pose_[1]= -0.045;//-0.045;//0.005 -1.8797 0.005;1.981  0.005
  robot_pose_[2]=0.0;  
 
  robot_vel_[0]=0;
  robot_vel_[2]=0;
  last_theta = 0.0;
  count_imu=0;
  

  /**
   * Create parameters for the ros2 node. How are the parameters being used?
   */
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

  /**
   * Get the values of the parameters to set member variables
   */
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

/**
 * Create publishers for odometry, corrected odometry, joint states, and docking status.
 * These publishers use qos 5, however the variable qos is not used here.
 */

  // auto qos = rclcpp::QoS(rclcpp::KeepLast(10));
  auto qos = rclcpp::QoS(rclcpp::SensorDataQoS());
  odom_pub_ = nh_->create_publisher<nav_msgs::msg::Odometry>("odom", 5);
  corr_odom_pub_ = nh_->create_publisher<nav_msgs::msg::Odometry>("corr_motor_odom", 5);
  pub_ = nh_->create_publisher<sensor_msgs::msg::JointState>("joint_states_jetson", 5);
  docking_finished_pub_ = nh_->create_publisher<std_msgs::msg::Bool>("docking_finished", 1);

  this->is_initialized = false;

  tf_broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>(nh_);

  last_time=nh_->now();
  use_imu_ = false;
  if (use_imu_)
  {
    imu_sub_ = nh_->create_subscription<sensor_msgs::msg::Imu>(
      "imu",
      qos,
      std::bind(&Odometry::imu_callback, this, std::placeholders::_1));
    joint_state_sub_ = nh_->create_subscription<sensor_msgs::msg::JointState>(
      "joint_states",
      qos,
      std::bind(&Odometry::joint_state_callback, this, std::placeholders::_1));

  }
  else
  {
    joint_state_sub_ = nh_->create_subscription<sensor_msgs::msg::JointState>(
      "joint_states",
      qos,
      std::bind(&Odometry::joint_state_callback, this, std::placeholders::_1));
  }
  //initial_pose_sub_ = nh_->create_subscription<nav_msgs::msg::Odometry>(
  //  "slam_odom_global", rclcpp::SystemDefaultsQoS(),
  //  std::bind(&Odometry::initialPoseReceived, this, std::placeholders::_1));
  
  //this->startSlam();
 // publish(nh_->now());

  update_odometry_server_ = nh_->create_service<odometry_msgs::srv::UpdateOdometry>(
    "motors_node/update_odometry", 
    std::bind(&Odometry::UpdateOdometry, this, std::placeholders::_1, std::placeholders::_2));
}

void Odometry::initialPoseReceived(nav_msgs::msg::Odometry::SharedPtr msg){
  auto duration = rclcpp::Duration::from_nanoseconds( nh_->now().nanoseconds() - this->slam_initial_time.nanoseconds());
  double time = duration.seconds();

  //RCLCPP_INFO(nh_->get_logger(), "Time since slam initialization: [%f]", time);
  if ((this->is_initialized) || (time < 15)) return;
  //RCLCPP_INFO(nh_->get_logger(), "Inicializou odom!");

  this->is_initialized = true;
  robot_pose_[0]=msg->pose.pose.position.x; 
  robot_pose_[1]=msg->pose.pose.position.y;
  tf2::Matrix3x3 mat(tf2::Quaternion(msg->pose.pose.orientation.x, msg->pose.pose.orientation.y,
                                     msg->pose.pose.orientation.z, msg->pose.pose.orientation.w));
  double yaw, pitch, roll;
  mat.getEulerYPR(yaw, pitch, roll);
  robot_pose_[2]=yaw;
  //last_theta=0;

  this->stopSlam();
  this->initial_pose_sub_.reset();
}

void Odometry::UpdateOdometry(
  const std::shared_ptr<odometry_msgs::srv::UpdateOdometry::Request> request,
  std::shared_ptr<odometry_msgs::srv::UpdateOdometry::Response> response)
{
  robot_pose_[0]=request->pose.pose.position.x; 
  robot_pose_[1]=request->pose.pose.position.y;

  tf2::Matrix3x3 mat(tf2::Quaternion(request->pose.pose.orientation.x, 
                                     request->pose.pose.orientation.y,
                                     request->pose.pose.orientation.z, 
                                     request->pose.pose.orientation.w));

  double yaw, pitch, roll;
  mat.getEulerYPR(yaw, pitch, roll);
  robot_pose_[2]=yaw;

  publish_corr(last_time);

  response->success = true;
  RCLCPP_INFO(nh_->get_logger(), "sending back response: [%d]", response->success);

  std_msgs::msg::Bool is_docking_finished;
  is_docking_finished.data = true;

  docking_finished_pub_->publish(std::move(is_docking_finished));
}

void Odometry::startSlam()
{
  RCLCPP_INFO(this->nh_->get_logger(), "Starting slam!");
  this->slam_initial_time = this->nh_->now();
  //this->is_slam_init = true;
  std::system("ros2 launch scooby snoopy_samsung_localization_launch.py &");
}

void Odometry::stopSlam()
{
  RCLCPP_INFO(this->nh_->get_logger(), "Stopping slam!");
  //this->is_slam_init = false;
  std::system("killall cartographer_node");
}

void Odometry::imu_callback(const sensor_msgs::msg::Imu::SharedPtr imu_msg)
{
  update_imu(imu_msg);
}

void Odometry::joint_state_callback(const sensor_msgs::msg::JointState::SharedPtr joint_state_msg)
{
  
  rclcpp::Time timeARM =joint_state_msg->header.stamp;
  //rclcpp::Time time = nh_->now();//joint_state_msg->header.stamp;
  rclcpp::Time time = joint_state_msg->header.stamp;
  auto duration = rclcpp::Duration::from_nanoseconds(time.nanoseconds() - last_time.nanoseconds());
  //RCLCPP_INFO(nh_->get_logger(), "x : %d, y : %d", joint_state_msg->header.stamp.sec*1000000000 +joint_state_msg->header.stamp.nanosec - last_time.nanoseconds() , 0.0f );
  update_joint_state(joint_state_msg);

  calculate_odometry(duration);

  //Call Joint state publisher
  publishJointState(time,joint_state_msg);

  
 publish(time);
//  rclcpp::Time timeEND=nh_->now();
//  rclcpp::Duration duration2(timeARM.nanoseconds() - time.nanoseconds());
//  rclcpp::Duration duration3(timeEND.nanoseconds() -time.nanoseconds());


//  std::cout<<duration2.seconds()<<","<<duration3.seconds()
//           <<","<<duration.seconds()<<std::endl;

  last_time = time;
}



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

  corr_odom_pub_->publish(std::move(odom_msg));

 if (publish_tf_)
   tf_broadcaster_->sendTransform(odom_tf);
}

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

void Odometry::update_joint_state(
  const std::shared_ptr<sensor_msgs::msg::JointState const> &joint_state)
{
  static std::array<double, 2> last_joint_positions = {0.0f, 0.0f};

  diff_joint_positions_[0] = joint_state->position[0] - last_joint_positions[0];
  diff_joint_positions_[1] = joint_state->position[1] - last_joint_positions[1];

  //v_x = wheels_radius_ *(joint_state->velocity[0]+joint_state->velocity[1])/2;
  v_x = ((wheels_radius_left * joint_state->velocity[0]) + (wheels_radius_right * joint_state->velocity[1]))/2;
  //TODO:change direction for real robot
  //w_z = -1.0*wheels_radius_ *(joint_state->velocity[0]-joint_state->velocity[1])/(wheels_separation_);
  w_z = -1.0*(wheels_radius_left * joint_state->velocity[0]- wheels_radius_right * joint_state->velocity[1])/(wheels_separation_);

  last_joint_positions[0] = joint_state->position[0];
  last_joint_positions[1] = joint_state->position[1];
}

void Odometry::update_imu(const std::shared_ptr<sensor_msgs::msg::Imu const> &imu)
{
  imu_angle_ = atan2f(
    imu->orientation.x*imu->orientation.y + imu->orientation.w*imu->orientation.z,
    0.5f - imu->orientation.y*imu->orientation.y - imu->orientation.z*imu->orientation.z);

  imu_vel_ =imu->angular_velocity.z;
  imu_time_=imu->header.stamp;
}

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
