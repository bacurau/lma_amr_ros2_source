#ifndef CONVERT_JOYSTICK_COMMANDS_TO_CMD_VEL
#define CONVERT_JOYSTICK_COMMANDS_TO_CMD_VEL

#include <memory>
#include  "rclcpp/rclcpp.hpp"
#include <sensor_msgs/msg/joy.hpp>
#include <sensor_msgs/msg/joy_feedback.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <geometry_msgs/msg/twist_stamped.hpp>

// Buttons

#define X_BUTTON 0
#define A_BUTTON 1
#define B_BUTTON 2
#define Y_BUTTON 3
#define LB_BUTTON 4
#define RB_BUTTON 5
#define LT_BUTTON 6
#define RT_BUTTON 7
#define BACK_BUTTON 8
#define START_BUTTON 9
#define L3_BUTTON 10 // when you press the left analogic button, the round one that moves.
#define R3_BUTTON 11 // when you press the right analogic button, the round one that moves.

// Axis

#define LEFT_ANALOG_STICK_HORIZONTAL_MOVEMENT 0
#define LEFT_ANALOG_STICK_VERTICAL_MOVEMENT 1
#define RIGHT_ANALOG_STICK_HORIZONTAL_MOVEMENT 2
#define RIGHT_ANALOG_STICK_VERTICAL_MOVEMENT 3
#define DIRECTIONAL_PAD_HORIZONTAL_MOVEMENT 4
#define DIRECTIONAL_PAD_VERTICAL_MOVEMENT 5



#define BUTTON_PRESSED 1
#define BUTTON_NOT_PRESSED 0


namespace convert_joystick_to_cmdvel
{

class ConvertJoystickCommandsToCmdVel : public rclcpp::Node
{
public:
  explicit ConvertJoystickCommandsToCmdVel();
 
  // ~ConvertJoystickCommandsToCmdVel();

private:
  void joyCallback(sensor_msgs::msg::Joy::SharedPtr joy_msg);
  rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr subscriber_joy;
  sensor_msgs::msg::Joy joy_msg_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_cmd_vel_stamped;
  double temp_velocity=0;
  double minimum_velocity=0.01;
  double maximum_velocity=0.1;
  double velocity_increment= 0.01;
  int previous_start_button_state=BUTTON_PRESSED;
  bool rising_edge=false;
  bool pause=true;
  bool first_iteration=true;
};


}  // namespace joy

#endif  // JOY__JOY_HPP_
