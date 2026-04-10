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

class LatencyProtobufNestedTalker : public rclcpp::Node
{
public:
  explicit LatencyProtobufNestedTalker(const rclcpp::NodeOptions & options)
  : rclcpp::Node("latency_protobuf_nested_talker", options)
  {
    topic_ = this->declare_parameter<std::string>("topic", "latency_protobuf_nested");
    total_msgs_ = static_cast<size_t>(this->declare_parameter<int>("total_msgs", 5000));
    history_len_ = static_cast<size_t>(this->declare_parameter<int>("history_len", 50));
    blob_bytes_ = static_cast<size_t>(this->declare_parameter<int>("blob_bytes", 4096));
    period_ms_ = this->declare_parameter<int>("period_ms", 0);

    pub_ = this->create_publisher<demo_nodes_cpp::proto::Nested>(topic_, rclcpp::QoS(10));

    // Pre-fill static structure.
    msg_.mutable_pose()->mutable_position()->set_x(1.0);
    msg_.mutable_pose()->mutable_position()->set_y(2.0);
    msg_.mutable_pose()->mutable_position()->set_z(3.0);
    msg_.mutable_pose()->mutable_orientation_rpy()->set_x(0.1);
    msg_.mutable_pose()->mutable_orientation_rpy()->set_y(0.2);
    msg_.mutable_pose()->mutable_orientation_rpy()->set_z(0.3);
    msg_.mutable_twist()->mutable_linear()->set_x(0.1);
    msg_.mutable_twist()->mutable_linear()->set_y(0.2);
    msg_.mutable_twist()->mutable_linear()->set_z(0.3);
    msg_.mutable_twist()->mutable_angular()->set_x(0.01);
    msg_.mutable_twist()->mutable_angular()->set_y(0.02);
    msg_.mutable_twist()->mutable_angular()->set_z(0.03);

    msg_.mutable_blob()->assign(blob_bytes_, '\0');
    for (size_t i = 0; i < history_len_; ++i) {
      auto * p = msg_.add_history();
      p->mutable_position()->set_x(static_cast<double>(i));
      p->mutable_orientation_rpy()->set_z(static_cast<double>(i) * 0.01);
      auto * t = msg_.add_history_twist();
      t->mutable_linear()->set_x(static_cast<double>(i) * 0.001);
      t->mutable_angular()->set_z(static_cast<double>(i) * 0.002);
    }

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
    const int64_t now_ns = this->get_clock()->now().nanoseconds();
    msg_.set_publish_time_ns(now_ns);
    pub_->publish(msg_);
    ++sent_;
  }

  std::string topic_;
  size_t total_msgs_{0};
  size_t history_len_{0};
  size_t blob_bytes_{0};
  int period_ms_{0};

  size_t sent_{0};
  demo_nodes_cpp::proto::Nested msg_;
  rclcpp::Publisher<demo_nodes_cpp::proto::Nested>::SharedPtr pub_;
  rclcpp::TimerBase::SharedPtr timer_;
};

}  // namespace demo_nodes_cpp

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<demo_nodes_cpp::LatencyProtobufNestedTalker>(rclcpp::NodeOptions{}));
  rclcpp::shutdown();
  return 0;
}

