// Licensed under the Apache License, Version 2.0

#include <memory>
#include <string>
#include <vector>

#include "demo_nodes_cpp/latency_stats.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "rclcpp/rclcpp.hpp"

namespace demo_nodes_cpp
{

class LatencyRos2MsgNestedListener : public rclcpp::Node
{
public:
  explicit LatencyRos2MsgNestedListener(const rclcpp::NodeOptions & options)
  : rclcpp::Node("latency_ros2msg_nested_listener", options)
  {
    topic_ = this->declare_parameter<std::string>("topic", "latency_ros2msg_nested");
    total_msgs_ = static_cast<size_t>(this->declare_parameter<int>("total_msgs", 5000));
    warmup_msgs_ = static_cast<size_t>(this->declare_parameter<int>("warmup_msgs", 200));

    sub_ = this->create_subscription<nav_msgs::msg::Odometry>(
      topic_, rclcpp::QoS(10),
      [this](const nav_msgs::msg::Odometry & msg) { this->on_msg(msg); });
  }

private:
  void on_msg(const nav_msgs::msg::Odometry & msg)
  {
    const auto now = this->get_clock()->now();
    const double dt_us = (now - msg.header.stamp).seconds() * 1e6;

    ++received_;
    if (received_ <= warmup_msgs_) {
      return;
    }
    samples_us_.push_back(dt_us);
    if (samples_us_.size() >= total_msgs_) {
      auto s = compute_latency_stats_us(samples_us_);
      RCLCPP_INFO(
        this->get_logger(),
        "ROS2MSG nested: n=%zu mean=%.2fus p50=%.2fus p95=%.2fus p99=%.2fus max=%.2fus",
        s.count, s.mean_us, s.p50_us, s.p95_us, s.p99_us, s.max_us);
      rclcpp::shutdown();
    }
  }

  std::string topic_;
  size_t total_msgs_{0};
  size_t warmup_msgs_{0};
  size_t received_{0};
  std::vector<double> samples_us_;
  rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr sub_;
};

}  // namespace demo_nodes_cpp

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<demo_nodes_cpp::LatencyRos2MsgNestedListener>(rclcpp::NodeOptions{}));
  rclcpp::shutdown();
  return 0;
}

