#include <chrono>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  auto node = rclcpp::Node::make_shared("minimal_publisher");
  auto pub = node->create_publisher<std_msgs::msg::String>("chatter", 10);
  size_t count = 0;
  auto timer = node->create_wall_timer(
    500ms,
    [&]() {
      auto msg = std_msgs::msg::String();
      msg.data = "Hello, world: " + std::to_string(++count);
      RCLCPP_INFO(node->get_logger(), "Publishing: '%s'", msg.data.c_str());
      pub->publish(msg);
    });

  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
