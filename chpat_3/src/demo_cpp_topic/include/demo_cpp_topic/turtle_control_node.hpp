#ifndef DEMO_CPP_TOPIC_TURTLE_CONTROL_NODE_HPP_
#define DEMO_CPP_TOPIC_TURTLE_CONTROL_NODE_HPP_

#include <memory>
#include <string>
#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"
#include "turtlesim/msg/pose.hpp"

namespace demo_cpp_topic
{

class TurtleControlNode : public rclcpp::Node
{
public:
  explicit TurtleControlNode(const std::string & node_name);

private:
  void on_pose_received(const turtlesim::msg::Pose::SharedPtr msg);

  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
  rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr subscription_;
  double target_x_{5.0};
  double target_y_{5.0};
  double k_lin_{1.0};
  double max_speed_{4.0};
};

}  // namespace demo_cpp_topic

#endif  // DEMO_CPP_TOPIC_TURTLE_CONTROL_NODE_HPP_
