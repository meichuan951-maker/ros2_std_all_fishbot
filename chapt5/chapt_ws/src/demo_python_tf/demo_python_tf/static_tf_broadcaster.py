import rclpy
from rclpy.node import Node
from tf2_ros import StaticTransformBroadcaster
from geometry_msgs.msg import TransformStamped#消息接口
from tf2_ros import TransformBroadcaster#tf2_ros库中的TransformBroadcaster类，用于广播变换信息
from tf_transformations import quaternion_from_euler#欧拉角转四元数
import math#数学库


class StaticTFBroadcaster(Node):
    def __init__(self):
        super().__init__('static_tf_broadcaster')
        self.static_broadcaster = TransformBroadcaster(self)
        self.create_timer(1.0, self.publish_static_tf)#创建定时器，每1秒广播一次静态变换

    def publish_static_tf(self):
        #发布静态TF，从base_link到camera_link的坐标变换
        transform = TransformStamped()
        transform.header.frame_id = 'base_link'
        transform.child_frame_id = 'camera_link'
        transform.header.stamp = self.get_clock().now().to_msg()

        transform.transform.translation.x = 0.5
        transform.transform.translation.y = 0.3
        transform.transform.translation.z = 0.6
        #欧拉角，四元数
        quaternion = quaternion_from_euler(math.radians(180.0), 0, 0)
        #旋转部分进行赋值
        transform.transform.rotation.x = quaternion[0]
        transform.transform.rotation.y = quaternion[1]
        transform.transform.rotation.z = quaternion[2]
        transform.transform.rotation.w = quaternion[3]
        #广播变换信息
        self.static_broadcaster.sendTransform(transform)


def main():
    rclpy.init()
    node = StaticTFBroadcaster()
    rclpy.spin(node)
    rclpy.shutdown()
