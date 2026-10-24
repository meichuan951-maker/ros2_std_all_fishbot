# ROS 2 与 FishBot 学习记录

本仓库汇总 ROS 2、FishBot 和机器人规划相关的学习代码与实验资料，由 [meichuan951-maker](https://github.com/meichuan951-maker) 整理维护。

## 目录

- `chapt2/`：C++ 与 Python 节点基础。
- `chpat_3/`、`chpat_4/`：话题、服务及自定义接口练习。
- `chapt5/` 至 `chapt8/`：各章节工作空间及配套示例。
- `chapt9/`：FishBot 固件、传感器与 ROS 2 配套代码。
- `chapt10/`：进阶示例、通信配置及 rosbag 实验数据。
- `ego_planner_ws/`：EGO Planner 相关源码及学习工作空间。

目录名保留本地学习时的原始拼写。各工作空间独立构建；仓库仅保存源码与必要资料，构建结果 `build/`、`install/`、`log/` 不纳入版本控制。

## 使用

先安装相应 ROS 2 版本及各示例依赖，再进入需要运行的工作空间。典型 colcon 工作空间的构建方式为：

```bash
source /opt/ros/<你的发行版>/setup.bash
colcon build
source install/setup.bash
```

具体依赖、固件烧录方式及运行参数，请以各子目录中的说明、`package.xml` 和 `platformio.ini` 为准。

通过 Wi-Fi 连接 micro-ROS 的示例中，`YOUR_WIFI_SSID` 和 `YOUR_WIFI_PASSWORD` 需要替换为运行环境的实际配置；提交代码前请勿保留真实密码。

## 源码来源与许可

本仓库包含学习示例和第三方项目的源码快照。原有许可证、版权声明、作者信息和项目说明均予以保留；各部分代码遵循其所在目录的许可条款。仓库提交记录中的维护者身份不改变第三方源码的原始归属。
