// Licensed under the Apache License, Version 2.0

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>

#include "example/msg/big_bytes.hpp"
#include "rclcpp/rclcpp.hpp"

using namespace std::chrono_literals;

namespace example
{

class LatencyRos2MsgBigTalker : public rclcpp::Node
{
public:
  explicit LatencyRos2MsgBigTalker(const rclcpp::NodeOptions & options)
  : rclcpp::Node("latency_ros2msg_big_talker", options)
  {
    topic_ = this->declare_parameter<std::string>("topic", "latency_ros2msg_big");
    payload_bytes_ = static_cast<size_t>(this->declare_parameter<int>("payload_bytes", 1024 * 1024));
    total_msgs_ = static_cast<size_t>(this->declare_parameter<int>("total_msgs", 400));
    period_ms_ = this->declare_parameter<int>("period_ms", 5);

    pub_ = this->create_publisher<example::msg::BigBytes>(topic_, rclcpp::QoS(10));

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
    const int64_t now_ns = this->get_clock()->now().nanoseconds();
    msg_.publish_time_ns = now_ns;
    pub_->publish(msg_);
    ++sent_;
  }

  std::string topic_;
  size_t payload_bytes_{0};
  size_t total_msgs_{0};
  int period_ms_{1};

  size_t sent_{0};
  example::msg::BigBytes msg_;
  rclcpp::Publisher<example::msg::BigBytes>::SharedPtr pub_;
  rclcpp::TimerBase::SharedPtr timer_;
};

}  // namespace example

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<example::LatencyRos2MsgBigTalker>(rclcpp::NodeOptions{}));
  rclcpp::shutdown();
  return 0;
}
