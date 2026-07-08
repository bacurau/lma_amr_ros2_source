#ifndef IMU_COVARIANCE
#define IMU_COVARIANCE

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/imu.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "geometry_msgs/msg/vector3_stamped.hpp"
#include "tf2/utils.hpp"
#include "control_toolbox/low_pass_filter.hpp"
#include <cmath>
#include <queue>
#define PI 3.141592
#define WINDOW_SIZE 100
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
        void RemoveBias_and_Drift(const sensor_msgs::msg::Imu::SharedPtr imu_msg_from_sensor);
        void LiberateNewBiasCalculation(const rclcpp::Parameter & parameter);
        rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr subscriber_imu_from_sensor;
        rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr publisher_imu_with_changed_covariance_values;
        rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr publisher_odom_from_imu;
        rclcpp::Publisher<geometry_msgs::msg::Vector3Stamped>::SharedPtr publisher_correct_angular_velocities_from_imu;
        std::shared_ptr<rclcpp::ParameterEventHandler> param_subscriber_for_start_bias_calculation_for_imu;
        std::shared_ptr<rclcpp::ParameterCallbackHandle> bias_parameter_call_back_handle;
        rclcpp::Time last_time = rclcpp::Time(0);
        control_toolbox::LowPassFilter<std::vector<double>> low_pass_filter{400, 5, 1.0};
        double hypotenuse=0.0;
        double current_x_linear_acceleration=0.0;
        double current_yaw_rate=0.0;
        double current_yaw_angle=0.0;
        double x_pose=0.0;
        double y_pose=0.0;
        double previous_x_linear_velocity=0.0;
        double current_x_linear_velocity=0.0;
        int imu_window_size=0;
        double z_angular_velocity_bias=0.0;
        int use_bias=0;
        std::queue<double>z_angular_velocity_queue;
};


}

#endif