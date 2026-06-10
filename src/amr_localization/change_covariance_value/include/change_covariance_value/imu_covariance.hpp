#ifndef IMU_COVARIANCE
#define IMU_COVARIANCE

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/imu.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "tf2/utils.hpp"
#include <cmath>

#define PI 3.141592

namespace imu_covariance_namespace
{




class ImuCovariance: public rclcpp::Node
{
    public:
        explicit ImuCovariance();
        ~ImuCovariance(){};

    private:
        void GetImuMsgAndChangeCovarianceValues(const sensor_msgs::msg::Imu::SharedPtr imu_msg_from_sensor);
        void CalculateImuPose(const sensor_msgs::msg::Imu::SharedPtr imu_msg_from_sensor);
        rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr subscriber_imu_from_sensor;
        rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr publisher_imu_with_changed_covariance_values;
        rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr publiser_odom_from_imu;
        rclcpp::Time last_time = rclcpp::Time(0);
        double hypotenuse=0.0;
        double current_x_linear_acceleration=0.0;
        double current_yaw_rate=0.0;
        double current_yaw_angle=0.0;
        double x_pose=0.0;
        double y_pose=0.0;
        double previous_x_linear_velocity=0.0;
        double current_x_linear_velocity=0.0;
};


}

#endif