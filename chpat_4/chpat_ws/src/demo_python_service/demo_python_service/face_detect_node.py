import rclpy
from rclpy.node import Node
from rcl_interfaces.msg import SetParametersResult
from chpat_4_interfaces.srv import FaceDerector
import face_recognition
import cv2
from ament_index_python.packages import get_package_share_directory#获取share包的绝对路径
import os
from cv_bridge import CvBridge
import time


class FaceDetectNode(Node):
    def __init__(self):
        super().__init__('face_detect_node')
        self.bridge_ = CvBridge()
        self.service_ = self.create_service(FaceDerector, 'face_detect', self.face_detect_callback)
        self.default_image_path = os.path.join(get_package_share_directory
        ('demo_python_service'), 'resource/default.jpg')
        self.get_logger().info("人脸识别服务已启动")
        self.declare_parameter('number_of_times_to_upsample', 1)
        self.declare_parameter('model', 'hog')
        self.number_of_times_to_upsample_ = self.get_parameter('number_of_times_to_upsample').value
        self.model_ = self.get_parameter('model').value
        self.add_on_set_parameters_callback(self.parameter_callback)

    def parameter_callback(self, parameters):
        for parameter in parameters:
            self.get_logger().info(f"参数: {parameter.name} -> {parameter.value}")
            if parameter.name == 'number_of_times_to_upsample':
                self.number_of_times_to_upsample_ = parameter.value
            if parameter.name == 'model':
                self.model_ = parameter.value
        return SetParametersResult(successful=True)

    def face_detect_callback(self, request, response):
        if request.image.data:
            cv_image = self.bridge_.imgmsg_to_cv2(request.image)
        else:
            cv_image = cv2.imread(self.default_image_path)
            self.get_logger().info("传入图像为空，使用默认图像")
        start_time = time.time()
        self.get_logger().info(f"加载完成图像，开始识别{start_time}")        

        face_locations = face_recognition.face_locations(cv_image,number_of_times_to_upsample = self.number_of_times_to_upsample_, model=self.model_)
        response.use_time = time.time() - start_time
        response.number = len(face_locations)

        for top , right , bottom , left in face_locations:
            response.top.append(top)
            response.right.append(right)
            response.bottom.append(bottom)
            response.left.append(left)

        
        return response #返回响应



def main():
    rclpy.init()
    node = FaceDetectNode()
    rclpy.spin(node)
    rclpy.shutdown()

