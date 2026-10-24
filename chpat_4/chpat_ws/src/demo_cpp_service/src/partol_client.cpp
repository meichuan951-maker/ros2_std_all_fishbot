#include "rclcpp/rclcpp.hpp"
#include "chpat_4_interfaces/srv/partol.hpp"
#include <chrono>
#include <ctime>
#include"rcl_interfaces/msg/parameter.hpp"
#include"rcl_interfaces/msg/parameter_type.hpp"
#include"rcl_interfaces/msg/parameter_value.hpp"
#include"rcl_interfaces/srv/set_parameters.hpp"

using SetParameters = rcl_interfaces::srv::SetParameters;
using Partol = chpat_4_interfaces::srv::Partol;
using namespace std::chrono_literals;


class PartolClient : public rclcpp::Node
{
public:
    PartolClient() : Node("partol_client")
    {
        srand(time(NULL));
        partol_client_ = this->create_client<Partol>("partol");
        timer_ = this->create_wall_timer(10s, 
        [&]() -> void{
            //1.检测服务端是否上下线
            while (!this->partol_client_->wait_for_service(1s)){
                if(!rclcpp::ok()){
                    RCLCPP_ERROR(this->get_logger(), "等待服务上线的过程中，rclcpp挂了，我退下了。。。。");
                    return;
                }
                RCLCPP_INFO(this->get_logger(), "服务未上线，等待中。。。。");
            }
            //2.创建请求
            auto request = std::make_shared<Partol::Request>();
            request->target_x = rand() % 15;
            request->target_y = rand() % 15;
            RCLCPP_INFO(this->get_logger(), "请求目标点: x = %f, y = %f", request->target_x, 
            request->target_y);
            //3.发送请求
            this->partol_client_->async_send_request(request, [this]
                (rclcpp::Client<Partol>::SharedFuture result_future) -> void{
                    auto response = result_future.get();
                    if(response->result == Partol::Response::SUCESS){
                        RCLCPP_INFO(this->get_logger(), "请求成功，服务器已接收目标点");
                    }else{
                        RCLCPP_ERROR(this->get_logger(), "请求失败，目标点超出范围");
                    }
                });
        });
    }
//创建客户端set_parameters_client_,发送请求，返回结果
    SetParameters::Response::SharedPtr call_set_parameters(const rcl_interfaces::msg::Parameter parameter){
        auto set_parameters_client = this->create_client<SetParameters>("/turtle_control/set_parameters");
            //1.检测服务端是否上下线
            while (!set_parameters_client->wait_for_service(1s)){
                if(!rclcpp::ok()){
                    RCLCPP_ERROR(this->get_logger(), "等待服务上线的过程中，rclcpp挂了，我退下了。。。。");
                    return nullptr;
                }
                RCLCPP_INFO(this->get_logger(), "服务未上线，等待中。。。。");
            }
            //2.创建请求
            auto request = std::make_shared<SetParameters::Request>();
            request->parameters.push_back(parameter);
            //3.发送请求
            auto future = set_parameters_client->async_send_request(request);
            rclcpp::spin_until_future_complete(this->get_node_base_interface(), future);
            auto response = future.get();
            return response;
    }
//更新参数
    void update_server_param_k(double k){
            //创建一个参数对象
            auto parameter = rcl_interfaces::msg::Parameter();
            parameter.name = "k";
            auto param_value = rcl_interfaces::msg::ParameterValue();
            param_value.type = rcl_interfaces::msg::ParameterType::PARAMETER_DOUBLE;
            param_value.double_value = k;
            parameter.value = param_value;
            //请求更新参数
            auto response = call_set_parameters(parameter);
            if(response == nullptr){
                RCLCPP_ERROR(this->get_logger(), "请求更新参数失败");
                return;
            }
            for(auto result : response->results){
                if(result.successful){
                    RCLCPP_INFO(this->get_logger(), "参数更新成功, k 新值: %f", k);
                }else{
                    RCLCPP_ERROR(this->get_logger(), "参数更新失败,原因: %s", result.reason.c_str());
                }
            }
        }
private:
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Client<Partol>::SharedPtr partol_client_;

};

int main(int argc,char *argv[]){
    rclcpp::init(argc,argv);
    auto node = std::make_shared<PartolClient>();
    node->update_server_param_k(4.0);
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}