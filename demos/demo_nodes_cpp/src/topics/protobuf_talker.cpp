// Copyright 2026
// Licensed under the Apache License, Version 2.0

#include <chrono>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"

#include "simple.pb.h"

using namespace std::chrono_literals;

namespace demo_nodes_cpp
{

class ProtobufTalker : public rclcpp::Node
{
public:
  explicit ProtobufTalker(const rclcpp::NodeOptions & options)
  : rclcpp::Node("protobuf_talker", options)
  {
    publisher_ = this->create_publisher<demo_nodes_cpp::proto::SimpleString>("protobuf_chatter", 10);
    timer_ = this->create_wall_timer(500ms, [this]() { this->on_timer(); });
  }

private:
  void on_timer()
  {
    demo_nodes_cpp::proto::SimpleString msg;
    msg.set_data("hello from protobuf_talker: " + std::to_string(count_++));
    publisher_->publish(msg);
    RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", msg.data().c_str());
  }

  size_t count_{0};
  rclcpp::Publisher<demo_nodes_cpp::proto::SimpleString>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
};

}  // namespace demo_nodes_cpp

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<demo_nodes_cpp::ProtobufTalker>(rclcpp::NodeOptions{}));
  rclcpp::shutdown();
  return 0;
}

