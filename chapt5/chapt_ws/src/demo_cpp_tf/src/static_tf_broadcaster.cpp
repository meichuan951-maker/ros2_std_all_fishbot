#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"//提供消息接口
#include "tf2_ros/static_transform_broadcaster.hpp"//提供静态坐标变换广播器
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"//提供四元数与欧拉角之间的转换
#include "tf2/LinearMath/Quaternion.h"//提供四元数


class StaticTFBroadcaster : public rclcpp::Node
{
private:
    std::shared_ptr<tf2_ros::StaticTransformBroadcaster> broadcaster_;


public:
    StaticTFBroadcaster() : Node("static_tf_broadcaster"){
        broadcaster_ = std::make_shared<tf2_ros::StaticTransformBroadcaster>(this);
        


    };

    void public_tf(){
        geometry_msgs::msg::TransformStamped transform;
        transform.header.stamp = this->get_clock()->now();
        transform.header.frame_id = "map";
        transform.child_frame_id = "target_point";
        transform.transform.translation.x = 9.0;
        transform.transform.translation.y = 5.0;
        transform.transform.translation.z = 0.0;

        tf2::Quaternion q;
        q.setRPY(0.0, 0.0, 60*M_PI/180.0);
        transform.transform.rotation = tf2::toMsg(q);
        broadcaster_->sendTransform(transform);
    }

    void timer_callback(){
        public_tf();
    }
};


int main(int argc, char *argv[]){
    rclcpp::init(argc, argv);
    auto node = std::make_shared<StaticTFBroadcaster>();
    rclcpp::TimerBase::SharedPtr timer = node->create_wall_timer(std::chrono::seconds(1), 
    std::bind(&StaticTFBroadcaster::timer_callback, node));
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}