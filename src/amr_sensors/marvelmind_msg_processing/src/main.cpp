#include "marvelmind_msg_processing/marvelmind_adapter.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv); 

  rclcpp::executors::SingleThreadedExecutor executor;

  auto marvelmind_adapter_node = 
    std::make_shared<lma::scooby::MarvelmindAdapter>(); /*!< Create object of Odometry Class in node format for ROS2*/

  executor.add_node(marvelmind_adapter_node); /*!< Add node to executor*/
  executor.spin();

  rclcpp::shutdown();
  return 0;
}
