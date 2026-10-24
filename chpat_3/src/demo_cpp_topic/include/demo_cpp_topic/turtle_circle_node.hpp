#ifndef DEMO_CPP_TOPIC_TURTLE_CIRCLE_NODE_HPP_
#define DEMO_CPP_TOPIC_TURTLE_CIRCLE_NODE_HPP_

#include <chrono>
#include <string>
#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"

namespace demo_cpp_topic
{

class TurtleCircleNode : public rclcpp::Node
{
public:
  explicit TurtleCircleNode(const std::string & node_name);

private:
  void timer_callback();

  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
};

}  // namespace demo_cpp_topic

#endif  // DEMO_CPP_TOPIC_TURTLE_CIRCLE_NODE_HPP_
