#include "demo_cpp_topic/turtle_control_node.hpp"
#include <algorithm>
#include <cmath>
#include <functional>

using demo_cpp_topic::TurtleControlNode;

TurtleControlNode::TurtleControlNode(const std::string & node_name)
: Node(node_name)
{
  publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("turtle1/cmd_vel", 10);
  subscription_ = this->create_subscription<turtlesim::msg::Pose>(
    "turtle1/pose", 10,
    std::bind(&TurtleControlNode::on_pose_received, this, std::placeholders::_1));
}

void TurtleControlNode::on_pose_received(const turtlesim::msg::Pose::SharedPtr msg)
{
  const double dx = target_x_ - msg->x;
  const double dy = target_y_ - msg->y;
  const double distance = std::hypot(dx, dy);
  const double target_theta = std::atan2(dy, dx);
  const double yaw_error = std::atan2(std::sin(target_theta - msg->theta), std::cos(target_theta - msg->theta));

  geometry_msgs::msg::Twist message;
  if (distance > 0.1) {
    message.linear.x = std::min(max_speed_, k_lin_ * distance);
    message.angular.z = std::min(max_speed_, 2.0 * yaw_error);
  }

  RCLCPP_INFO(get_logger(), "pose=(%.2f,%.2f,%.2f) dist=%.2f yaw_err=%.2f lin=%.2f ang=%.2f",
    msg->x, msg->y, msg->theta, distance, yaw_error, message.linear.x, message.angular.z);

  publisher_->publish(message);
}

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<TurtleControlNode>("turtle_control_node");
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
