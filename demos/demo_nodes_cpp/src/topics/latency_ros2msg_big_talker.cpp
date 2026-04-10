// Licensed under the Apache License, Version 2.0

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"

using namespace std::chrono_literals;

namespace demo_nodes_cpp
{

class LatencyRos2MsgBigTalker : public rclcpp::Node
{
public:
  explicit LatencyRos2MsgBigTalker(const rclcpp::NodeOptions & options)
  : rclcpp::Node("latency_ros2msg_big_talker", options)
  {
    topic_ = this->declare_parameter<std::string>("topic", "latency_ros2msg_big");
    payload_bytes_ = static_cast<size_t>(this->declare_parameter<int>("payload_bytes", 1024 * 1024));
    total_msgs_ = static_cast<size_t>(this->declare_parameter<int>("total_msgs", 200));
    period_ms_ = this->declare_parameter<int>("period_ms", 1);

    pub_ = this->create_publisher<sensor_msgs::msg::Image>(topic_, rclcpp::QoS(10));

    msg_.header.frame_id = "latency";
    msg_.height = 1;
    msg_.width = static_cast<uint32_t>(payload_bytes_);
    msg_.encoding = "8UC1";
    msg_.is_bigendian = false;
    msg_.step = msg_.width;
    msg_.data.resize(payload_bytes_, 0u);

    timer_ = this->create_wall_timer(
      std::chrono::milliseconds(std::max(1, period_ms_)),
      [this]() { this->tick(); });
  }

private:
  void tick()
  {
    if (sent_ >= total_msgs_) {
      rclcpp::shutdown();
      return;
    }
    msg_.header.stamp = this->get_clock()->now();
    pub_->publish(msg_);
    ++sent_;
  }

  std::string topic_;
  size_t payload_bytes_{0};
  size_t total_msgs_{0};
  int period_ms_{1};

  size_t sent_{0};
  sensor_msgs::msg::Image msg_;
  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr pub_;
  rclcpp::TimerBase::SharedPtr timer_;
};

}  // namespace demo_nodes_cpp

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<demo_nodes_cpp::LatencyRos2MsgBigTalker>(rclcpp::NodeOptions{}));
  rclcpp::shutdown();
  return 0;
}

