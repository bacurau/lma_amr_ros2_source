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

#include "scooby_node/diff_drive_controller.hpp"

using namespace lma;
using namespace scooby;


  /**
 * \brief Constructor of DiffDriveController class
 * \details 1- Creates a shared pointer (DiffDriveController::nh_) that points do this class object. <br>
 *          2- Define the time source as the topic /clock with use+sim_time. <br>
 *          3- Creates an instance of the Odometry class that receives DiffDriveController::nh_ as a parameter.
 */
DiffDriveController::DiffDriveController()
: Node("snoopy_odom", rclcpp::NodeOptions().use_intra_process_comms(true))
{
   nh_ = std::shared_ptr<::rclcpp::Node>(this, [](::rclcpp::Node *) {});

  rclcpp::Parameter simTime( "use_sim_time", rclcpp::ParameterValue( true ) );
  nh_->set_parameter( simTime );
  //auto useSimTime = nh_->get_parameter( "use_sim_time" ).as_bool();

  odometry_ = std::make_unique<Odometry>(
    nh_); /*!< making odometry a unique pointer guarantees that only DiffDriveController::odometry_ will be able to access this object */

  RCLCPP_INFO(this->get_logger(), "Run!");
}