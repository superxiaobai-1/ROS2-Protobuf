// Copyright 2026
// Licensed under the Apache License, Version 2.0

#include <memory>

#include "rclcpp/rclcpp.hpp"

#include "simple.pb.h"

namespace example
{

class ProtobufListener : public rclcpp::Node
{
public:
  explicit ProtobufListener(const rclcpp::NodeOptions & options)
  : rclcpp::Node("protobuf_listener", options)
  {
    subscription_ = this->create_subscription<example::proto::SimpleString>(
      "protobuf_chatter",
      10,
      [this](const example::proto::SimpleString & msg)
      {
        RCLCPP_INFO(this->get_logger(), "I heard: '%s'", msg.data().c_str());
      });
  }

private:
  rclcpp::Subscription<example::proto::SimpleString>::SharedPtr subscription_;
};

}  // namespace example

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<example::ProtobufListener>(rclcpp::NodeOptions{}));
  rclcpp::shutdown();
  return 0;
}

