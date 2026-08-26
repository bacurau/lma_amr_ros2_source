#include "change_covariance_value/imu_covariance.hpp"


using namespace imu_covariance_namespace;



ImuCovariance::ImuCovariance(): rclcpp::Node("imu_covariance_calculator_node"){
  this->subscriber_imu_from_sensor = this->create_subscription<sensor_msgs::msg::Imu>(
    "/imu/data", rclcpp::SensorDataQoS(), std::bind(&ImuCovariance::ProcessImuMsg,this, std::placeholders::_1));
  this->publisher_imu_with_changed_covariance_values = create_publisher<sensor_msgs::msg::Imu>("new_imu_data",rclcpp::SensorDataQoS());
  this->publisher_odom_from_imu = create_publisher<nav_msgs::msg::Odometry>("odom_from_imu",rclcpp::SensorDataQoS());

  this->param_subscriber_for_start_bias_calculation_for_imu = std::make_shared<rclcpp::ParameterEventHandler>(this);
  bias_parameter_call_back_handle = this->param_subscriber_for_start_bias_calculation_for_imu->add_parameter_callback("odometry.start_bias_calculation_for_imu",
    std::bind(&ImuCovariance::LiberateNewBiasCalculation,this, std::placeholders::_1),
    "odometry_node");
  
}

void ImuCovariance::ProcessImuMsg(sensor_msgs::msg::Imu::SharedPtr imu_msg_from_sensor ){
  ChangeCovarianceValues(imu_msg_from_sensor);
  RemoveBias_and_Drift(imu_msg_from_sensor);
  CalculateImuPose(imu_msg_from_sensor);
  auto imu_msg = std::make_unique<sensor_msgs::msg::Imu>(*imu_msg_from_sensor);
  imu_msg->header.stamp = this->get_clock()->now();
  publisher_imu_with_changed_covariance_values->publish(std::move(imu_msg));
   
}

void ImuCovariance::ChangeCovarianceValues(sensor_msgs::msg::Imu::SharedPtr imu_msg_with_0_covariance){
  static double angular_z_velocity_window_sum = 0;
  z_angular_velocity_queue_for_variance.push_back(imu_msg_with_0_covariance->angular_velocity.z);
  int queue_size = z_angular_velocity_queue_for_variance.size();
  if(queue_size>VARIANCE_WINDOW_SIZE){
    angular_z_velocity_window_sum-=z_angular_velocity_queue_for_variance.front();
    z_angular_velocity_queue_for_variance.pop_front();
  }
  angular_z_velocity_window_sum+=imu_msg_with_0_covariance->angular_velocity.z;
  double mean = angular_z_velocity_window_sum/queue_size;

  // sum of 0 to N of (xi - mean)^2
  double angular_velocity_z_variance = 0;
  if(queue_size>1){
    for (int z: z_angular_velocity_queue_for_variance){
      angular_velocity_z_variance+= (z-mean)*(z-mean);
    }
    angular_velocity_z_variance/=(queue_size-1);
  }

  imu_msg_with_0_covariance->angular_velocity_covariance[0] = 1e-2;
  imu_msg_with_0_covariance->angular_velocity_covariance[4] = 1e-2;
  imu_msg_with_0_covariance->angular_velocity_covariance[8] = angular_velocity_z_variance;
  
  imu_msg_with_0_covariance->linear_acceleration_covariance[0] = 1e-2;
  imu_msg_with_0_covariance->linear_acceleration_covariance[4] = 1e-2;
  imu_msg_with_0_covariance->linear_acceleration_covariance[8] = 1e-4;
}

void ImuCovariance::RemoveBias_and_Drift(sensor_msgs::msg::Imu::SharedPtr imu_msg_with_bias){
   static double sum_of_z_angular_velocities = 0;
  // remove bias when robot is static.
  sum_of_z_angular_velocities+=imu_msg_with_bias->angular_velocity.z;
  z_angular_velocity_queue.push(imu_msg_with_bias->angular_velocity.z);
  if(z_angular_velocity_queue.size()>WINDOW_SIZE){
    sum_of_z_angular_velocities-=z_angular_velocity_queue.front();
    z_angular_velocity_queue.pop();
  }
 
  if(use_bias && z_angular_velocity_queue.size()==WINDOW_SIZE){
    imu_msg_with_bias->angular_velocity.z-=sum_of_z_angular_velocities/(double)WINDOW_SIZE;
    //RCLCPP_INFO(this->get_logger(), "Filtered data: %lf. Removed bias: %lf",filtered_data[2],sum_of_z_angular_velocities/20.0);
  }
}



void ImuCovariance::CalculateImuPose(const sensor_msgs::msg::Imu::SharedPtr imu_msg_from_sensor){
  
  // angular velocity from imu is in radian/s  
  rclcpp::Time time = imu_msg_from_sensor->header.stamp;
  //double bias = -0.05;
  current_x_linear_acceleration = imu_msg_from_sensor->linear_acceleration.x;
  current_yaw_rate = imu_msg_from_sensor->angular_velocity.z;
  auto duration = rclcpp::Duration::from_nanoseconds(time.nanoseconds() - last_time.nanoseconds());
  double delta_t =  duration.seconds();

  current_x_linear_acceleration = std::abs(current_x_linear_acceleration) > 0.05?current_x_linear_acceleration:0.0;
  current_yaw_rate = std::abs(current_yaw_rate)>0.010?current_yaw_rate:0.0;

  if(last_time.nanoseconds() > 0){
    current_x_linear_velocity = previous_x_linear_velocity + current_x_linear_acceleration*delta_t;
    current_x_linear_velocity = std::abs(current_x_linear_velocity)>0.0010?current_x_linear_velocity:0.0;
    current_yaw_angle =  std::fmod(current_yaw_angle + current_yaw_rate*delta_t,2.0*PI);
    double delta_s = previous_x_linear_velocity*delta_t + (current_x_linear_acceleration*delta_t*delta_t)*0.5; // s = s_i +v_i*t + (a*t^2)/2
    x_pose += -cos(current_yaw_angle)* delta_s;
    y_pose += sin(current_yaw_angle)* delta_s;

    auto odometry_msg_from_imu = std::make_unique<nav_msgs::msg::Odometry>();
    odometry_msg_from_imu->header.frame_id = "odom_frame";
    odometry_msg_from_imu->child_frame_id  = "Base_Link";
    odometry_msg_from_imu->header.stamp = time;
    odometry_msg_from_imu->pose.pose.position.x = x_pose;
    odometry_msg_from_imu->pose.pose.position.y = y_pose;
    odometry_msg_from_imu->pose.pose.position.z = 0.0;

    tf2::Quaternion q;
    q.setRPY(0.0, 0.0, current_yaw_angle);

    odometry_msg_from_imu->pose.pose.orientation.x = q.x();
    odometry_msg_from_imu->pose.pose.orientation.y = q.y();
    odometry_msg_from_imu->pose.pose.orientation.z = q.z();
    odometry_msg_from_imu->pose.pose.orientation.w = q.w();

    odometry_msg_from_imu->twist.twist.linear.x  = current_x_linear_velocity;
    odometry_msg_from_imu->twist.twist.linear.y  = previous_x_linear_velocity;
    odometry_msg_from_imu->twist.twist.angular.z = current_yaw_rate;
    odometry_msg_from_imu->twist.twist.angular.y = current_x_linear_acceleration;

    publisher_odom_from_imu->publish(std::move(odometry_msg_from_imu));
  }
  previous_x_linear_velocity = current_x_linear_velocity;
  last_time = time;
}
/*

  Description:  This function is called when there is an update in the parameter odometry.start_bias_calculation_for_imu. 
                It is responsible for locking or unlocking the bias recalculation for the imu.
                This parameter is set by the odometry class in the function: Odometry::liberate_imu_bias_calculation.

*/
void ImuCovariance::LiberateNewBiasCalculation(const rclcpp::Parameter & p){ 
  use_bias=p.as_int();
}

int main(int argc, char ** argv)
{
  (void) argc;
  (void) argv;

  rclcpp::init(argc, argv); 

  rclcpp::executors::SingleThreadedExecutor executor;

  auto imu_covariance_modifier_node = 
    std::make_shared<ImuCovariance>(); /*!< Create object of Odometry Class in node format for ROS2*/

  executor.add_node(imu_covariance_modifier_node); /*!< Add node to executor*/
  executor.spin();

  rclcpp::shutdown();
  return 0;
}
