#ifndef CONVERT_JOYSTICK_COMMANDS_TO_CMD_VEL
#define CONVERT_JOYSTICK_COMMANDS_TO_CMD_VEL

#include <memory>
#include  "rclcpp/rclcpp.hpp"
#include <sensor_msgs/msg/joy.hpp>
#include <sensor_msgs/msg/joy_feedback.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <geometry_msgs/msg/twist_stamped.hpp>

// Buttons

#define X_BUTTON 3
#define A_BUTTON 0
#define B_BUTTON 1
#define Y_BUTTON 2
#define LB_BUTTON 4
#define RB_BUTTON 5
#define LT_BUTTON 6
#define RT_BUTTON 7
#define BACK_BUTTON 8
#define START_BUTTON 9
#define L3_BUTTON 10 // when you press the left analogic button, the round one that moves.
#define R3_BUTTON 11 // when you press the right analogic button, the round one that moves.

// Playstation controller buttons
#define DIRECTIONAL_PAD_UP 13
#define DIRECTIONAL_PAD_DOWN 14
#define DIRECTIONAL_PAD_LEFT 15
#define DIRECTIONAL_PAD_RIGHT 16




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
  void setLinearVelocityWithComplementaryFilter(const bool forward, const bool backward);
  void setAngularVelocityWithComplementaryFilter(const bool turn_right, const bool turn_left);
  void setLinearVelocityToReachWithLimits(const int velocity_increment_signal);
  void setAngularVelocityToReachWithLimits(const int velocity_increment_signal);
  // ~ConvertJoystickCommandsToCmdVel();

private:
  void joyCallback(sensor_msgs::msg::Joy::SharedPtr joy_msg);
  rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr subscriber_joy;
  sensor_msgs::msg::Joy joy_msg_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_cmd_vel_stamped;
  int previous_start_button_state=BUTTON_PRESSED;
  bool rising_edge=false;
  bool pause=true;
  bool first_iteration=true;
  // Simulated robot speed parameters, change them as you see fit.
  // Be careful with these parameters when you are using the real robot.
  double current_linear_velocity=0.0;
  double current_angular_velocity=0.0;
  double maximum_linear_velocity=2; // 5 ms per second is the maximum linear velocity the simulated robot should achieve. 
  double minimum_linear_velocity=0.0;
  double maximum_angular_velocity=1.5708/2.0; // 45 degrees in radians
  double minimum_angular_velocity=0.0;
  double linear_velocity_increment= maximum_linear_velocity/20;
  double angular_velocity_increment= maximum_angular_velocity/20;
  double linear_velocity_to_reach=0.0;
  double angular_velocity_to_reach=0.0;
  const double alpha=0.7; // the complementary filter parameter, change it as you see fit. It should be between 0 and 1.
  double epsilon=1e-3; // because of the complementary filter, the velocity never reaches 0. To make sure the velocity is set to 0, a threshold is used.
   //------ Buttons used to change robot speed ----------
  int velocity_increment_signal = 1;
};


}  // namespace joy

#endif  // JOY__JOY_HPP_
