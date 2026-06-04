#ifndef LOCALIZATION_ERROR_CALCULATOR
#define LOCALIZATION_ERROR_CALCULATOR

#include "rclcpp/rclcpp.hpp"
#include "nav_msgs/msg/odometry.hpp"

namespace localization_error_calculator
{

class LocalizationErrorCalculator: public rclcpp::Node
{
    public:
        explicit LocalizationErrorCalculator();
        ~LocalizationErrorCalculator(){};

    private:
        void GetEkfOdometryMsgCallback(const nav_msgs::msg::Odometry::SharedPtr ekf_odometry_msg);
        void GetGazeboPerfectOdometryMsgCallback(const nav_msgs::msg::Odometry::SharedPtr gazebo_perfect_odometry_msg);
        void CalculateLocalizationError();
        rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr subscriber_odometry_from_ekf;
        rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr subscriber_perfect_odometry_from_gazebo;
        rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr publisher_localization_error;
        std::queue<nav_msgs::msg::Odometry> ekf_odom_msg_queue;
        std::queue<nav_msgs::msg::Odometry> gazebo_perfect_odometry_msg;
        double localization_error = 0.0;

};


}

#endif