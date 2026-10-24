import rclpy
from rclpy.node import Node
from tf2_ros import TransformListener, Buffer
from tf_transformations import euler_from_quaternion


class TFListener(Node):
    def __init__(self):
        super().__init__('tf_listener')
        self.buffer = Buffer()
        self.listener = TransformListener(self.buffer, self)
        self.timer = self.create_timer(1.0, self.get_transform)
        self.get_logger().info('TF 监听器已启动')

    def get_transform(self):
        # 查询 camera_link 到 bottle_link 的坐标变换
        try:
            result = self.buffer.lookup_transform(
                'camera_link',
                'bottle_link',
                rclpy.time.Time(),
                rclpy.duration.Duration(seconds=1.0))
            self.get_logger().info(f'平移信息：{result.transform.translation}')
            self.get_logger().info(f'旋转信息：{result.transform.rotation}')
            rotation = euler_from_quaternion([
                result.transform.rotation.x,
                result.transform.rotation.y,
                result.transform.rotation.z,
                result.transform.rotation.w,
            ])
            self.get_logger().info(f'欧拉角信息：{rotation}')
        except Exception as e:
            self.get_logger().error(f'获取变换信息失败：{e}')


def main():
    rclpy.init()
    node = TFListener()
    rclpy.spin(node)
    rclpy.shutdown()
