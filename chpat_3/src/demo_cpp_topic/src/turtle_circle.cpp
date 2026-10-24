#include"rclcpp/rclcpp.hpp"
#include"geometry_msgs/msg/twist.hpp"
#include <chrono>

class TurtleCircleNode : public rclcpp::Node
{
private:
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;

    void publish_velocity()
    {
        auto message = geometry_msgs::msg::Twist();
        message.linear.x = 1.0;  // Set linear velocity
        message.angular.z = 1.0; // Set angular velocity
        publisher_->publish(message);
    }

public:
    explicit TurtleCircleNode(const std::string & node_name) : Node(node_name)
    {
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("turtle1/cmd_vel", 10);
        timer_ = this->create_wall_timer(std::chrono::milliseconds(1000), std::bind(&TurtleCircleNode::timer_callback, this));
    }

    void timer_callback()
    {
        auto message = geometry_msgs::msg::Twist();
        message.linear.x = 1.0;  // Set linear velocity
        message.angular.z = 0.5; // Set angular velocity
        publisher_->publish(message);
    }


};


int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);


    auto node = std::make_shared<TurtleCircleNode>("turtle_circle_node");
    
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}