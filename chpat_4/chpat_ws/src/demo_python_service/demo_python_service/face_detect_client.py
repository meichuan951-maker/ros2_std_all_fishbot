import rclpy
from rclpy.node import Node
from chpat_4_interfaces.srv import FaceDerector
import face_recognition
import cv2
from ament_index_python.packages import get_package_share_directory#获取share包的绝对路径
import os
from cv_bridge import CvBridge
import time
from rcl_interfaces.srv import SetParameters
from rcl_interfaces.msg import Parameter, ParameterValue, ParameterType


class FaceDetectClient(Node):
    def __init__(self):
        super().__init__('face_detect_client')
        self.bridge_ = CvBridge()
        self.client = self.create_client(FaceDerector, 'face_detect')
        self.default_image_path = os.path.join(get_package_share_directory
        ('demo_python_service'), 'resource/test1.jpg')
        self.get_logger().info("人脸识别服务客户端已启动")
        self.image = cv2.imread(self.default_image_path)

    def call_set_parameter(self, parameters):
        #调用服务，修改参数值
        update_param = self.create_client(SetParameters, '/face_detect_node/set_parameters')
        while update_param.wait_for_service(timeout_sec=1.0) is False:
            self.get_logger().info('等待参数更新服务器上线')

        #构造请求
        request = SetParameters.Request()
        request.parameters = parameters
        #调用服务端更新参数
        future = update_param.call_async(request)
        rclpy.spin_until_future_complete(self, future)
        response = future.result()
        return response
#更新检测模型
    def update_detect_model(self,model = 'hog'):
        param = Parameter()
        param.name = 'model'
        #将模型名称转换为字符串,赋值
        param_value = ParameterValue()
        param_value.string_value = model
        param_value.type = ParameterType.PARAMETER_STRING
        param.value = param_value

        response = self.call_set_parameter([param])
        for result in response.results:
            self.get_logger().info(f"参数更新结果: successful = {result.successful}")

    def send_request(self):
        #1.判断在线
        while self.client.wait_for_service(timeout_sec=1.0) is False:
            self.get_logger().info("服务未上线，等待中...")
        #2.构造request
        request = FaceDerector.Request()
        request.image = self.bridge_.cv2_to_imgmsg(self.image)
        #3.发送请求，并等待处理
        future = self.client.call_async(request)#创建一个服务请求的异步回调函数，等待服务响应
        rclpy.spin_until_future_complete(self, future)#等待处理完成
        response = future.result()#获取服务响应
        self.get_logger().info(f"识别到{response.number}个人脸,用时{response.use_time}秒")
        #self.show_response(response)        
    


    def show_response(self, response):
        for i in range(response.number):
            top = response.top[i]
            right = response.right[i]
            bottom = response.bottom[i]
            left = response.left[i]
            cv2.rectangle(self.image, (left, top), (right, bottom), (0, 0, 255), 4)
        cv2.imshow("Face Detection", self.image)
        cv2.waitKey(0)



def main():
    rclpy.init()
    node = FaceDetectClient()
    node.update_detect_model('hog')
    node.send_request()
    node.update_detect_model('cnn')
    rclpy.spin(node)
    rclpy.shutdown()
