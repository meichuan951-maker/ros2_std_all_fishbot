import os
import rclpy
from rclpy.node import Node
from example_interfaces.msg import String
from queue import Queue

class NovelPubNode(Node):
    def __init__(self, node_name):
        super().__init__(node_name)
        self.get_logger().info(f'Node {node_name} has been started.启动了')
        self.get_logger().info('正在加载小说内容...')
        self.novels_queue = Queue()
        self.novel_publisher = self.create_publisher(String, 'novel', 10)
        self.timer_period = 1.0  # seconds
        self.timer = self.create_timer(self.timer_period, self.timer_callback)

    def timer_callback(self):
        if self.novels_queue.qsize() > 0:
            line = self.novels_queue.get()
            msg = String()
            msg.data = line
            self.novel_publisher.publish(msg)
            self.get_logger().info(f'已发布小说内容: {line}')
        else:
            self.get_logger().debug('timer tick')

    def load_file(self, path):
        if not os.path.isfile(path):
            raise FileNotFoundError(f'小说文件不存在: {path}')

        with open(path, 'r', encoding='utf-8') as f:
            lines = f.read().splitlines()

        self.get_logger().info(f'小说加载完成，共 {len(lines)} 行。')
        for line in lines:
            if line.strip():
                self.novels_queue.put(line)


def main():
    rclpy.init()
    node = NovelPubNode('novel_pub')
    try:
        package_dir = os.path.dirname(__file__)
        novel_file = os.path.join(package_dir, 'novels', 'novel1.txt')
        node.load_file(novel_file)
        rclpy.spin(node)
    except Exception as e:
        node.get_logger().error(f'运行时出错: {e}')
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()