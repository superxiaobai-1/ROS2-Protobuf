// Copyright 2026
// Licensed under the Apache License, Version 2.0

#include <memory>

#include "rclcpp/rclcpp.hpp"

#include "simple.pb.h"

namespace demo_nodes_cpp
{

class ProtobufListener : public rclcpp::Node
{
public:
  explicit ProtobufListener(const rclcpp::NodeOptions & options)
  : rclcpp::Node("protobuf_listener", options)
  {
    subscription_ = this->create_subscription<demo_nodes_cpp::proto::SimpleString>(
      "protobuf_chatter",
      10,
      [this](const demo_nodes_cpp::proto::SimpleString & msg)
      {
        RCLCPP_INFO(this->get_logger(), "I heard: '%s'", msg.data().c_str());
      });
  }

private:
  rclcpp::Subscription<demo_nodes_cpp::proto::SimpleString>::SharedPtr subscription_;
};

}  // namespace demo_nodes_cpp

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<demo_nodes_cpp::ProtobufListener>(rclcpp::NodeOptions{}));
  rclcpp::shutdown();
  return 0;
}

