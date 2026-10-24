# FishBot 巡检小车

基于 **ROS 2 Humble** 与 **Gazebo** 的室内自主巡检仿真项目。差速底盘 FishBot 搭载激光雷达、深度相机和 IMU，使用 **Nav2** 完成定位、路径规划与点到点导航。小车按 YAML 中的路点循环巡逻，到达站点后调用语音服务播报，并把当前相机画面保存为图片。

适合用来练习：URDF/xacro 建模、Gazebo 仿真、SLAM 建图、Nav2 导航，以及用 `nav2_simple_commander` 写巡检应用。

---

## 工作区结构

```
chapt7_ws/src/
├── fishbot_description        # 机器人 URDF、Gazebo 仿真 launch
├── fishbot_navigation2        # Nav2 参数、房间地图、导航 launch
├── fishbot_application        # Python：初始化位姿、单点导航、路点跟随
├── fishbot_application_cpp    # C++ 单点导航示例
├── autopatrol_interfaces      # 自定义服务 SpeachText.srv
└── autoparltrol_robot         # 巡检节点 + 语音节点 + launch
```

注意：巡逻功能包的 ROS 包名是 `autoparltrol_robot`（拼写与教程标准名略有不同），启动时必须用这个名字。

---

## 依赖

- ROS 2 Humble
- `gazebo_ros`、`nav2_bringup`、`slam_toolbox`（建图时用）
- Python：`nav2_simple_commander`、`cv_bridge`、`espeakng`

语音节点需要：

```bash
pip3 install espeakng
sudo apt install espeak-ng
```

---

## 编译

```bash
cd ~/ros2_stduy/chapt7/chapt7_ws
colcon build
source install/setup.bash
```

接口包要先于巡逻包编译。单独编译可以用：

```bash
colcon build --packages-select autopatrol_interfaces autoparltrol_robot fishbot_description fishbot_navigation2
source install/setup.bash
```

---

## 启动命令

按顺序开三个终端，**不要同时开 SLAM 和 Nav2**（都会占 `/map`）。

### 1. 仿真（Gazebo，不带 RViz）

```bash
cd ~/ros2_stduy/chapt7/chapt7_ws
source install/setup.bash
ros2 launch fishbot_description gazebo_sim.launch.py
```

### 2. 导航（Nav2 + RViz）

```bash
cd ~/ros2_stduy/chapt7/chapt7_ws
source install/setup.bash
ros2 launch fishbot_navigation2 navgation2.launch.py
```

RViz 里可用 **2D Pose Estimate** 给 AMCL 一个初始位姿；不给则 TF 中没有 `map` 坐标系，巡检节点会一直报找不到 `map → base_footprint`。

### 3. 巡检

```bash
cd ~/ros2_stduy/chapt7/chapt7_ws
source install/setup.bash
ros2 launch autoparltrol_robot autopartol.launch.py
```

该 launch 会同时启动：

- `patrol_node`：读路点、导航、拍照、请求语音
- `speaker`：提供 `/speech_text` 服务，用 eSpeak 朗读

修改巡逻点：编辑 `src/autoparltrol_robot/config/partol_config.yaml` 后重新 `colcon build --packages-select autoparltrol_robot`。

### 其它常用命令

```bash
# 键盘遥控（建图时用）
ros2 run teleop_twist_keyboard teleop_twist_keyboard

# 在线建图
ros2 launch slam_toolbox online_async_launch.py use_sim_time:=true

# 保存地图到导航包
cd ~/ros2_stduy/chapt7/chapt7_ws/src/fishbot_navigation2/maps
ros2 run nav2_map_server map_saver_cli -f room

# 单点导航（Python）
ros2 run fishbot_application nav_to_pose

# 路点跟随（Python）
ros2 run fishbot_application waypoint_follower

# 查询机器人在地图中的位姿
ros2 run fishbot_application get_robot_pose
```

---

## 巡检流程

```
初始化语音 → setInitialPose → 等待 Nav2 Active
        ↓
   读取 target_points
        ↓
  对每个路点：语音播报 → goToPose → 到达后拍照存图
        ↓
      循环下一圈
```

路点在 `partol_config.yaml` 中按 `[x, y, yaw, ...]` 每 3 个数一组：

```yaml
patrol_node:
  ros__parameters:
    initial_point: [0.0, 0.0, 0.0]
    target_points: [
      0.0, 0.0, 0.0,
      1.0, 2.0, 3.14,
      -4.5, 1.5, 1.57,
      -8.0, -5.0, 1.57,
      1.0, -5.0, 3.14,
    ]
```

---

## 重点代码

### 1. 巡逻主循环 `partol_node.py`

继承 `BasicNavigator`，把「初始化 → 逐点导航 → 语音 → 拍照」串在一起：

```153:170:src/autoparltrol_robot/autopartol_robot/partol_node.py
def main():
    rclpy.init()
    patrol = PatrolNode()
    patrol.speach_text(text='正在初始化位置')
    patrol.init_robot_pose()
    patrol.speach_text(text='位置初始化完成')

    while rclpy.ok():
        for point in patrol.get_target_points():
            x, y, yaw = point[0], point[1], point[2]
            target_pose = patrol.get_pose_by_xyyaw(x, y, yaw)
            patrol.speach_text(text=f'准备前往目标点{x},{y}')
            patrol.nav_to_pose(target_pose)
            patrol.speach_text(text=f"已到达目标点{x},{y},准备记录图像")
            patrol.record_image()
            patrol.speach_text(text=f"图像记录完成")
```

`nav_to_pose()` 内部调用 Nav2 的 `goToPose()`，并打印预计到达时间。

### 2. 语音服务 `speaker.py` + `SpeachText.srv`

自定义服务：请求里是要朗读的文字，响应里是是否成功。

```
string text    # 合成文字
---
bool result    # 合成结果
```

`speaker` 节点创建服务 `/speech_text`，巡逻节点作为客户端调用：

```6:19:src/autoparltrol_robot/autopartol_robot/speaker.py
class Speaker(Node):
    def __init__(self, node_name):
        super().__init__(node_name)
        self.speech_service = self.create_service(
            SpeachText, 'speech_text', self.speak_text_callback)
        self.speaker = espeakng.Speaker()
        self.speaker.voice = 'zh'

    def speak_text_callback(self, request, response):
        self.get_logger().info('正在朗读 %s' % request.text)
        self.speaker.say(request.text)
        self.speaker.wait()
        response.result = True
        return response
```

### 3. 到站拍照 `record_image()`

订阅 `/camera_sensor/image_raw`，到达目标后用 TF 当前位置命名并写盘：

```40:47:src/autoparltrol_robot/autopartol_robot/partol_node.py
    def record_image(self):
        if self.latest_image is not None:
          pose = self.get_current_pose()
          cv_image = self.bridge.imgmsg_to_cv2(self.latest_image)
          cv2.imwrite(f'{self.image_save_path}image_{pose.translation.x:3.2f}_{pose.translation.y:3.2f}.png', cv_image)
```

### 4. 导航启动 `navgation2.launch.py`

加载房间地图 `maps/room.yaml` 和 `config/nav2_params.yaml`，并带上 Nav2 默认 RViz 配置。

### 5. 仿真启动 `gazebo_sim.launch.py`

启动 Gazebo、生成 FishBot、加载 `ros2_control` 差速控制器。RViz 已从该 launch 中去掉，避免和导航 RViz 抢窗口。

---

## TF 关系（巡检依赖）

```
map → odom → base_footprint → base_link → laser_link / camera_link / ...
         ↑              ↑
       AMCL        差速控制器 + robot_state_publisher
```

- `/map` 栅格图由 `map_server` 发布
- `map` 坐标系由 **AMCL 收到初始位姿之后** 才出现
- 巡检节点用 `lookup_transform('map', 'base_footprint')` 取当前位姿

---

## 常见问题

| 现象 | 原因 |
|---|---|
| `package 'autopatrol_robot' not found` | 实际包名是 `autoparltrol_robot` |
| `No module named 'autopatrol_interfaces'` | 接口包未编译，先 `colcon build --packages-select autopatrol_interfaces` |
| `target_frame map does not exist` | 未给 AMCL 初始位姿，或导航没启动 |
| speaker 报 `espeakng` | 未安装 Python 包 / 系统 `espeak-ng` |
