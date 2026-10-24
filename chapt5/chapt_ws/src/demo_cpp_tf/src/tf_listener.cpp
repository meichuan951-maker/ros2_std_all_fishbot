#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "tf2_ros/transform_listener.hpp"
#include "tf2_ros/buffer.h"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"
#include "tf2/LinearMath/Quaternion.h"
#include "tf2/utils.h"
#include "tf2/exceptions.h"
#include "tf2/time.h"
#include <chrono>
#include <memory>

using namespace std::chrono_literals;

class TFListener : public rclcpp::Node
{
private:
    std::shared_ptr<tf2_ros::Buffer> buffer_;
    std::shared_ptr<tf2_ros::TransformListener> listener_;
    rclcpp::TimerBase::SharedPtr timer_;

public:
    TFListener() : Node("tf_listener")
    {
        buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
        listener_ = std::make_shared<tf2_ros::TransformListener>(*buffer_, this);
        timer_ = this->create_wall_timer(6s, std::bind(&TFListener::get_transform, this));
    }

    void get_transform()
    {
        try {
            const auto transform = buffer_->lookupTransform(
                "base_link",
                "target_point",
                tf2::TimePointZero,
                tf2::durationFromSec(1.0));
            auto translation = transform.transform.translation;
            auto rotation = transform.transform.rotation;
            double yaw, pitch, roll;
            tf2::getEulerYPR(rotation, yaw, pitch, roll);
            RCLCPP_INFO(this->get_logger(), "平移距离: %f, %f, %f",
                translation.x, translation.y, translation.z);
            RCLCPP_INFO(this->get_logger(), "旋转角度Yaw: %f, Pitch: %f, Roll: %f",
                yaw, pitch, roll);
        } catch (const tf2::TransformException & e) {
            RCLCPP_ERROR(this->get_logger(), "%s", e.what());
        }
    }
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TFListener>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
