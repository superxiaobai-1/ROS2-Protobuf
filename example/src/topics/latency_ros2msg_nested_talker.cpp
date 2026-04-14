// Licensed under the Apache License, Version 2.0

#include <chrono>
#include <memory>
#include <string>

#include "nav_msgs/msg/odometry.hpp"
#include "rclcpp/rclcpp.hpp"

using namespace std::chrono_literals;

namespace example
{

class LatencyRos2MsgNestedTalker : public rclcpp::Node
{
public:
  explicit LatencyRos2MsgNestedTalker(const rclcpp::NodeOptions & options)
  : rclcpp::Node("latency_ros2msg_nested_talker", options)
  {
    topic_ = this->declare_parameter<std::string>("topic", "latency_ros2msg_nested");
    total_msgs_ = static_cast<size_t>(this->declare_parameter<int>("total_msgs", 6000));
    period_ms_ = this->declare_parameter<int>("period_ms", 5);

    pub_ = this->create_publisher<nav_msgs::msg::Odometry>(topic_, rclcpp::QoS(10));

    msg_.header.frame_id = "odom";
    msg_.child_frame_id = "base_link";
    msg_.pose.pose.position.x = 1.0;
    msg_.pose.pose.position.y = 2.0;
    msg_.pose.pose.position.z = 3.0;
    msg_.pose.pose.orientation.w = 1.0;
    msg_.twist.twist.linear.x = 0.1;
    msg_.twist.twist.angular.z = 0.2;

    timer_ = this->create_wall_timer(
      std::chrono::milliseconds(std::max(0, period_ms_)),
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
  size_t total_msgs_{0};
  int period_ms_{0};
  size_t sent_{0};
  nav_msgs::msg::Odometry msg_;
  rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr pub_;
  rclcpp::TimerBase::SharedPtr timer_;
};

}  // namespace example

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<example::LatencyRos2MsgNestedTalker>(rclcpp::NodeOptions{}));
  rclcpp::shutdown();
  return 0;
}

