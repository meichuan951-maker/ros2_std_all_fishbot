#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"
#include <cmath>
#include "turtlesim/msg/pose.hpp"
#include "rcl_interfaces/msg/set_parameters_result.hpp"
#include "chpat_4_interfaces/srv/partol.hpp"
using Partol = chpat_4_interfaces::srv::Partol;
using SetParametersResult = rcl_interfaces::msg::SetParametersResult;

class TurtleControl : public rclcpp::Node
{
public:
    TurtleControl() : Node("turtle_control")
    {
        this->declare_parameter("k", 1.0);
        this->declare_parameter("max_speed", 1.0);
        this->get_parameter("k", k_);
        this->get_parameter("max_speed", max_speed_);
        this->set_parameter(rclcpp::Parameter("k", 1.0));
        this->set_parameter(rclcpp::Parameter("max_speed", 1.0));

        on_set_parameters_callback_handle_ = this->add_on_set_parameters_callback(
            [&](const std::vector<rclcpp::Parameter> &parameters) -> SetParametersResult{
                rcl_interfaces::msg::SetParametersResult result;
                result.successful = true;
                RCLCPP_INFO(this->get_logger(), "更新参数的值：%s %s", parameters[0].get_name().c_str(), parameters[0].value_to_string().c_str());
                for(const auto &parameter : parameters){
                    if(parameter.get_name() == "k"){
                        k_ = parameter.as_double();
                    }
                    if(parameter.get_name() == "max_speed"){
                        max_speed_ = parameter.as_double();
                    }
                }
                return result;
            }
        );

        partol_service_ = this->create_service<Partol>("partol", [&](const Partol::Request::SharedPtr request,
             const Partol::Response::SharedPtr response) -> void{
                if((0 < request->target_x && request->target_x < 12.0f)
                && (0 < request->target_y && request->target_y < 12.0f)){
                this->target_x_ = request->target_x;
                this->target_y_ = request->target_y;
                response->result = Partol::Response::SUCESS;
                }else{
                    response->result = Partol::Response::FAIL;
                }
        });
        velocity_publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("turtle1/cmd_vel", 10);
        pose_subscriber_ = this->create_subscription<turtlesim::msg::Pose>("turtle1/pose", 10, 
            std::bind(&TurtleControl::on_pose_received, this, std::placeholders::_1));
    }
private:
    void on_pose_received(const turtlesim::msg::Pose::SharedPtr pose)
    {
        auto message = geometry_msgs::msg::Twist();

        double current_x_ = pose->x;
        double current_y_ = pose->y;
        RCLCPP_INFO(this->get_logger(), "Current position: x = %f, y = %f", current_x_, current_y_);

        double distance = std::sqrt(std::pow(current_x_ - target_x_, 2) + std::pow(current_y_ - target_y_, 2));
        double angle = std::atan2(target_y_ - current_y_, target_x_ - current_x_) - pose->theta;

        if (distance > 0.1){
            if(std::fabs(angle) > 0.2){
                message.angular.z = std::fabs(angle);
            }else{
                message.linear.x = k_ * distance;
            }
            if(message.linear.x > max_speed_){
                message.linear.x = max_speed_;
            }
            velocity_publisher_->publish(message);
        }
    }
private:
    OnSetParametersCallbackHandle::SharedPtr on_set_parameters_callback_handle_;
    rclcpp::Service<Partol>::SharedPtr partol_service_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr velocity_publisher_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr pose_subscriber_;
    double target_x_{1.0};
    double target_y_{1.0};
    double k_{0.5};
    double max_speed_{1.0};

};

int main(int argc,char *argv[]){
    rclcpp::init(argc,argv);
    auto node = std::make_shared<TurtleControl>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}