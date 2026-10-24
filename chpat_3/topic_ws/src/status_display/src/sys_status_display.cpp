#include <QApplication>
#include <QLabel>
#include <QString>
#include <rclcpp/rclcpp.hpp>
#include <status_interfaces/msg/system_status.hpp>

#include <memory>
#include <sstream>
#include <thread>

using SystemStatus = status_interfaces::msg::SystemStatus;

class SysStatusDisplay : public rclcpp::Node
{
public:
  explicit SysStatusDisplay(QLabel * label)
  : Node("sys_status_display"), label_(label)
  {
    subscriber_ = this->create_subscription<SystemStatus>(
      "system_status", 10,
      [this](const SystemStatus::SharedPtr msg) {
        label_->setText(get_qstr_from_msg(msg));
      });
    label_->setText(QStringLiteral("Waiting for system_status..."));
    label_->show();
  }

private:
  QString get_qstr_from_msg(const SystemStatus::SharedPtr msg)
  {
    std::stringstream show_str;
    show_str
      << "==============系统状态可视化工具============\n"
      << "数据时间戳:" << msg->stamp.sec << "." << msg->stamp.nanosec << '\n'
      << "主机名:" << msg->host_name << '\n'
      << "CPU使用率:" << msg->cpu_percent << "%\n"
      << "内存使用率:" << msg->memory_percent << "%\n"
      << "内存总量:" << msg->memory_total << "MB\n"
      << "内存剩余:" << msg->memory_available << "MB\n"
      << "内存已用:" << msg->memory_used << "MB\n"
      << "网络发送量:" << msg->net_bytes_sent << "MB\n"
      << "网络接收量:" << msg->net_bytes_recv << "MB\n";
    return QString::fromStdString(show_str.str());
  }

  rclcpp::Subscription<SystemStatus>::SharedPtr subscriber_;
  QLabel * label_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  QApplication app(argc, argv);
  QLabel * label = new QLabel();
  auto node = std::make_shared<SysStatusDisplay>(label);
  std::thread spin_thread([node]() {
      rclcpp::spin(node);
    });
  spin_thread.detach();
  const int ret = app.exec();
  rclcpp::shutdown();
  return ret;
}
