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

#include "scooby_serial/scooby.hpp"
#include <string>

using namespace std::chrono_literals;

Scooby::Scooby()
: Node("reset_node",rclcpp::NodeOptions().use_intra_process_comms(true))
{
  RCLCPP_INFO(get_logger(), "Reset Node Main");
  init_serial_wrapper();
  run();
}

void Scooby::init_serial_wrapper()
{
  my_serial.setPort("/dev/ttyUSB1");
  my_serial.setBaudrate(400);
  //void 	setTimeout (uint32_t inter_byte_timeout, uint32_t read_timeout_constant, uint32_t read_timeout_multiplier, uint32_t write_timeout_constant, uint32_t write_timeout_multiplier)
  my_serial.setTimeout(serial::Timeout::max(),10, 0, 10, 0);
  
  try {
    my_serial.open();
    //sleep(2);
    std::cout << " serial ports ready "<< my_serial.getPort()<<std::endl;
  
   //LOG_DEBUG("SerialWrapper", "Success to open Port");
  } catch (std::exception &e) {
     std::cout  << "Reset Exception: " << e.what() << std::endl;
   //  LOG_DEBUG("SerialWrapper", "Simulation mode");
  }
}

void Scooby::run()
{
  RCLCPP_INFO(this->get_logger(), "Reset!");
  my_serial.write("Reset arm");
}

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Scooby>());
  rclcpp::shutdown();

  return 0;
}
