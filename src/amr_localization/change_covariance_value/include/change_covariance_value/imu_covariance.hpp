#ifndef IMU_COVARIANCE
#define IMU_COVARIANCE

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/imu.hpp"

namespace imu_covariance_namespace
{

class ImuCovariance: public rclcpp::Node
{
    public:
        explicit ImuCovariance();
        ~ImuCovariance(){};

    private:
        void GetImuMsgAndChangeCovarianceValues(const sensor_msgs::msg::Imu::SharedPtr imu_msg_from_sensor);
        rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr subscriber_imu_from_sensor;
        rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr publisher_imu_with_changed_covariance_values;

};


}

#endif