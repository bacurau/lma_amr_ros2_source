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

#include <chrono>
#include <memory>
#include <string>

#include <rcutils/cmdline_parser.h>
#include <rclcpp/rclcpp.hpp>

#include "scooby_node/diff_drive_controller.hpp"


int main(int argc, char *argv[])
{
  setvbuf(stdout, NULL, _IONBF, BUFSIZ);

 
  rclcpp::init(argc, argv); 

  rclcpp::executors::SingleThreadedExecutor executor;

  //auto scooby = std::make_shared<lma::scooby::Scooby>("/dev/ttyACM1");
  // auto diff_drive_controller =
  //   std::make_shared<lma::scooby::DiffDriveController>(
  //     scooby->get_wheels()->separation,
  //     scooby->get_wheels()->radius);

  auto diff_drive_controller =
    std::make_shared<lma::scooby::DiffDriveController>();


  //executor.add_node(scooby);
  executor.add_node(diff_drive_controller);
  executor.spin();

  rclcpp::shutdown();

  return 0;
}