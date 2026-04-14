// Licensed under the Apache License, Version 2.0

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "example/latency_stats.hpp"
#include "example/msg/big_bytes.hpp"
#include "rclcpp/rclcpp.hpp"

namespace example
{

class LatencyRos2MsgBigListener : public rclcpp::Node
{
public:
  explicit LatencyRos2MsgBigListener(const rclcpp::NodeOptions & options)
  : rclcpp::Node("latency_ros2msg_big_listener", options)
  {
    topic_ = this->declare_parameter<std::string>("topic", "latency_ros2msg_big");
    total_msgs_ = static_cast<size_t>(this->declare_parameter<int>("total_msgs", 200));
    warmup_msgs_ = static_cast<size_t>(this->declare_parameter<int>("warmup_msgs", 20));

    sub_ = this->create_subscription<example::msg::BigBytes>(
      topic_, rclcpp::QoS(10),
      [this](const example::msg::BigBytes & msg) { this->on_msg(msg); });
  }

private:
  void on_msg(const example::msg::BigBytes & msg)
  {
    const int64_t now_ns = this->get_clock()->now().nanoseconds();
    const int64_t dt_ns = now_ns - msg.publish_time_ns;
    const double dt_us = static_cast<double>(dt_ns) / 1e3;

    ++received_;
    if (received_ <= warmup_msgs_) {
      return;
    }
    samples_us_.push_back(dt_us);
    if (samples_us_.size() >= total_msgs_) {
      auto s = compute_latency_stats_us(samples_us_);
      RCLCPP_INFO(
        this->get_logger(),
        "ROS2MSG big: n=%zu mean=%.2fus p50=%.2fus p95=%.2fus p99=%.2fus max=%.2fus",
        s.count, s.mean_us, s.p50_us, s.p95_us, s.p99_us, s.max_us);
      rclcpp::shutdown();
    }
  }

  std::string topic_;
  size_t total_msgs_{0};
  size_t warmup_msgs_{0};
  size_t received_{0};
  std::vector<double> samples_us_;
  rclcpp::Subscription<example::msg::BigBytes>::SharedPtr sub_;
};

}  // namespace example

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<example::LatencyRos2MsgBigListener>(rclcpp::NodeOptions{}));
  rclcpp::shutdown();
  return 0;
}
