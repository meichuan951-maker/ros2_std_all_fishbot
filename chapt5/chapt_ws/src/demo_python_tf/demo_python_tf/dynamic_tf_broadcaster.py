import rclpy
from rclpy.node import Node
from tf2_ros import TransformBroadcaster
from geometry_msgs.msg import TransformStamped#消息接口
from tf2_ros import TransformBroadcaster#tf2_ros库中的TransformBroadcaster类，用于广播变换信息
from tf_transformations import quaternion_from_euler#欧拉角转四元数
import math#数学库



class DynamicTFBroadcaster(Node):
    def __init__(self):
        super().__init__('dynamic_tf_broadcaster')
        self.broadcaster = TransformBroadcaster(self)
        self.timer = self.create_timer(1.0, self.publish_dynamic_tf)#创建定时器，每1秒广播一次动态变换
        self.get_logger().info('动态 TF 广播器已启动')


    def publish_dynamic_tf(self):
        #发布动态TF，从base_link到camera_link的坐标变换
        transform = TransformStamped()
        transform.header.frame_id = 'camera_link'
        transform.child_frame_id = 'bottle_link'
        transform.header.stamp = self.get_clock().now().to_msg()

        transform.transform.translation.x = 0.2
        transform.transform.translation.y = 0.3
        transform.transform.translation.z = 0.5
        #欧拉角，四元数
        quaternion = quaternion_from_euler(0, 0, 0)
        #旋转部分进行赋值
        transform.transform.rotation.x = quaternion[0]
        transform.transform.rotation.y = quaternion[1]
        transform.transform.rotation.z = quaternion[2]
        transform.transform.rotation.w = quaternion[3]
        #广播变换信息
        self.broadcaster.sendTransform(transform)
        self.get_logger().info('发布动态 TF 成功')


def main():
    rclpy.init()
    node = DynamicTFBroadcaster()
    rclpy.spin(node)
    rclpy.shutdown()
