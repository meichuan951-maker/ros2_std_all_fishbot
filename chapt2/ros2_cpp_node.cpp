#include <memory>
#include "rclcpp/rclcpp.hpp"

class MinimalNode : public rclcpp::Node
{
public:
  MinimalNode()
  : Node("ros2_cpp_node")
  {
    RCLCPP_INFO(this->get_logger(), "ros2_cpp_node started");
  }
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<MinimalNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
