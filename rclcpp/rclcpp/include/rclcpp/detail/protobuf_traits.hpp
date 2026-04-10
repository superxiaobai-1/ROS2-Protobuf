#ifndef RCLCPP__DETAIL__PROTOBUF_TRAITS_HPP_
#define RCLCPP__DETAIL__PROTOBUF_TRAITS_HPP_

#include <type_traits>

// Keep protobuf optional: compile-time detection via __has_include.
#if defined(__has_include)
#  if __has_include(<google/protobuf/message.h>)
#    include <google/protobuf/message.h>
#    define RCLCPP_DETAIL_HAS_PROTOBUF 1
#  else
#    define RCLCPP_DETAIL_HAS_PROTOBUF 0
#  endif
#else
#  define RCLCPP_DETAIL_HAS_PROTOBUF 0
#endif

namespace rclcpp
{
namespace detail
{

template<typename T, typename Enable = void>
struct is_protobuf_message : std::false_type {};

#if RCLCPP_DETAIL_HAS_PROTOBUF
template<typename T>
struct is_protobuf_message<
  T,
  std::enable_if_t<std::is_base_of<google::protobuf::Message, T>::value>
> : std::true_type {};
#endif

}  // namespace detail
}  // namespace rclcpp

#endif  // RCLCPP__DETAIL__PROTOBUF_TRAITS_HPP_

