#include <cstdio>

#include "joystick_to_cmd_vel/joystick_to_cmd_vel_node.hpp"

using namespace convert_joystick_to_cmdvel;

ConvertJoystickCommandsToCmdVel::ConvertJoystickCommandsToCmdVel()
: rclcpp::Node("convert_joystick_commands_to_cmd_vel_node"){


  this->subscriber_joy = this->create_subscription<sensor_msgs::msg::Joy>(
    "joy", rclcpp::QoS(3), // queue size
    std::bind(&ConvertJoystickCommandsToCmdVel::joyCallback, this, std::placeholders::_1));
  publisher_cmd_vel_stamped = this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 3); // queue size
}



void ConvertJoystickCommandsToCmdVel::joyCallback(const sensor_msgs::msg::Joy::SharedPtr joy_msg)
{

  if(first_iteration){
    previous_start_button_state=joy_msg->buttons[START_BUTTON];
    first_iteration=false;
    return;
  }
  // detecting a rising edge on the start button. 
  if(joy_msg->buttons[START_BUTTON] == BUTTON_PRESSED && previous_start_button_state==BUTTON_NOT_PRESSED){
    rising_edge=true;
  }  
  // detecting a falling edge on the start button. 
  else  if(joy_msg->buttons[START_BUTTON] == BUTTON_NOT_PRESSED && rising_edge==true) {
    rising_edge=false;
    pause=!pause ; 
  }
  previous_start_button_state=joy_msg->buttons[START_BUTTON]; 
  if(pause)return;
  
  auto message_to_publish_cmd_vel = std::make_unique<geometry_msgs::msg::Twist>();
  

  // ------ Buttons used to move the robot --------------
    // Turn right or left
  if(joy_msg->buttons[X_BUTTON] == BUTTON_PRESSED
  || joy_msg->buttons[DIRECTIONAL_PAD_LEFT] == BUTTON_PRESSED) message_to_publish_cmd_vel->angular.z=this->current_angular_velocity;
  else if(joy_msg->buttons[B_BUTTON] == BUTTON_PRESSED
  || joy_msg->buttons[DIRECTIONAL_PAD_RIGHT] == BUTTON_PRESSED) message_to_publish_cmd_vel->angular.z=-this->current_angular_velocity;
  // Move backward or forward
  if(joy_msg->buttons[Y_BUTTON] == BUTTON_PRESSED
  || joy_msg->buttons[DIRECTIONAL_PAD_UP] == BUTTON_PRESSED) message_to_publish_cmd_vel->linear.x=this->current_linear_velocity;
  else if(joy_msg->buttons[A_BUTTON] == BUTTON_PRESSED
  || joy_msg->buttons[DIRECTIONAL_PAD_DOWN] == BUTTON_PRESSED) message_to_publish_cmd_vel->linear.x=-this->current_linear_velocity;

  //------ Buttons used to change robot speed ----------
  if(joy_msg->buttons[LB_BUTTON] == BUTTON_PRESSED){
    this->current_linear_velocity-=linear_velocity_increment;
    this->current_angular_velocity-=angular_velocity_increment;
  } 
  if(joy_msg->buttons[RB_BUTTON] == BUTTON_PRESSED){
    this->current_linear_velocity+=linear_velocity_increment;
    this->current_angular_velocity+=angular_velocity_increment;
  } 

  // Check if maximum and minimum velocity are respected
  if(current_linear_velocity < minimum_linear_velocity) current_linear_velocity = minimum_linear_velocity;
  else if(current_linear_velocity > maximum_linear_velocity) current_linear_velocity = maximum_linear_velocity;
  if(current_angular_velocity < minimum_angular_velocity) current_angular_velocity = minimum_angular_velocity;
  else if(current_angular_velocity > maximum_angular_velocity) current_angular_velocity = maximum_angular_velocity;

  publisher_cmd_vel_stamped->publish(std::move(message_to_publish_cmd_vel));
}






int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  rclcpp::spin(std::make_unique<convert_joystick_to_cmdvel::ConvertJoystickCommandsToCmdVel>());

  rclcpp::shutdown();
  return 0;
}
