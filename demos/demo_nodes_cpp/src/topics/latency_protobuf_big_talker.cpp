// Licensed under the Apache License, Version 2.0

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "simple.pb.h"

using namespace std::chrono_literals;

namespace demo_nodes_cpp
{

class LatencyProtobufBigTalker : public rclcpp::Node
{
public:
  explicit LatencyProtobufBigTalker(const rclcpp::NodeOptions & options)
  : rclcpp::Node("latency_protobuf_big_talker", options)
  {
    topic_ = this->declare_parameter<std::string>("topic", "latency_protobuf_big");
    payload_bytes_ = static_cast<size_t>(this->declare_parameter<int>("payload_bytes", 1024 * 1024));
    total_msgs_ = static_cast<size_t>(this->declare_parameter<int>("total_msgs", 200));
    period_ms_ = this->declare_parameter<int>("period_ms", 1);

    pub_ = this->create_publisher<demo_nodes_cpp::proto::BigBytes>(topic_, rclcpp::QoS(10));

    msg_.set_data(std::string(payload_bytes_, '\0'));

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
    msg_.set_publish_time_ns(now_ns);
    pub_->publish(msg_);
    ++sent_;
  }

  std::string topic_;
  size_t payload_bytes_{0};
  size_t total_msgs_{0};
  int period_ms_{1};

  size_t sent_{0};
  demo_nodes_cpp::proto::BigBytes msg_;
  rclcpp::Publisher<demo_nodes_cpp::proto::BigBytes>::SharedPtr pub_;
  rclcpp::TimerBase::SharedPtr timer_;
};

}  // namespace demo_nodes_cpp

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<demo_nodes_cpp::LatencyProtobufBigTalker>(rclcpp::NodeOptions{}));
  rclcpp::shutdown();
  return 0;
}

