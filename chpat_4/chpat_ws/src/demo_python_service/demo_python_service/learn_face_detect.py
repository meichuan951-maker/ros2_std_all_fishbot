import face_recognition
import cv2
from ament_index_python.packages import get_package_share_directory#获取share包的绝对路径
import os
def main():
    #获取图片的真实路径
    default_image_path = get_package_share_directory('demo_python_service') + '/resource/default.jpg'

    print(f"default_image_path: {default_image_path}")
    #s使用cv2读取图片
    image = cv2.imread(default_image_path)
    face_locations = face_recognition.face_locations(image,number_of_times_to_upsample=1,
    model="hog")
#   遍历人脸位置，绘制人脸边框
    for top , right , bottom , left in face_locations:
        cv2.rectangle(image,(left,top),(right,bottom),(0,0,255),4)
        #结果显示
        cv2.imshow("Face Detection",image)
        cv2.waitKey(0)
        cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
