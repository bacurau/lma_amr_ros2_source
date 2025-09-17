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

#ifndef SCOOBY_NODE_SCOOBY_HPP_
#define SCOOBY_NODE_SCOOBY_HPP_

#include <array>
#include <chrono>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <queue>

#include <rclcpp/rclcpp.hpp>
#include <serial/serial.h>

class Scooby : public rclcpp::Node
{
 public:
  
  explicit Scooby();
  virtual ~Scooby(){};

 private:
  serial::Serial my_serial;

  void init_serial_wrapper();

  void run();
};

#endif // SCOOBY_NODE_SCOOBY_HPP_
