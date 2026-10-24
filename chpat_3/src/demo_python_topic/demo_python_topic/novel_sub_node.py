import espeakng
import rclpy
from rclpy.node import Node
from example_interfaces.msg import String
from queue import Queue
import threading
import time

class NovelSubNode(Node):
    def __init__(self, node_name):
        super().__init__(node_name)
        self.get_logger().info(f'Node {node_name} has been started.启动了')
        self.novels_queue = Queue()
        self.novel_subscriber = self.create_subscription(String,'novel',self.novel_callback,10)
        self.speech_thread = threading.Thread(target=self.speake_thread)
        self.speech_thread.start()
        
    def novel_callback(self, msg):    
        self.novels_queue.put(msg.data)

    def speake_thread(self):
        speaker = espeakng.Speaker()
        speaker.voice = 'zh'

        while rclpy.ok():
            if self.novels_queue.qsize() > 0:
                text = self.novels_queue.get()
                self.get_logger().info(f'正在朗读小说内容: {text}')
                speaker.say(text)
                speaker.wait()
            else:
                 time.sleep(1)  # Avoid busy waiting     

def main():
    rclpy.init()
    node = NovelSubNode('novel_sub')
    try:
        rclpy.spin(node)
    finally:
        rclpy.shutdown()


if __name__ == '__main__':
    main()
        