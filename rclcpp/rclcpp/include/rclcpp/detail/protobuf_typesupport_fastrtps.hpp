#ifndef RCLCPP__DETAIL__PROTOBUF_TYPESUPPORT_FASTRTPS_HPP_
#define RCLCPP__DETAIL__PROTOBUF_TYPESUPPORT_FASTRTPS_HPP_

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rclcpp/detail/protobuf_traits.hpp"

#if defined(__has_include)
#  if __has_include(<fastcdr/Cdr.h>) && __has_include(<rosidl_typesupport_fastrtps_cpp/message_type_support.h>) && __has_include(<rosidl_typesupport_fastrtps_cpp/identifier.hpp>) && __has_include(<rosidl_typesupport_introspection_cpp/identifier.hpp>) && __has_include(<rosidl_typesupport_introspection_cpp/message_introspection.hpp>) && __has_include(<rosidl_typesupport_introspection_c/identifier.h>)
#    define RCLCPP_DETAIL_HAS_FASTRTPS_TYPESUPPORT 1
#    include <fastcdr/Cdr.h>
#    include <rosidl_typesupport_fastrtps_cpp/identifier.hpp>
#    include <rosidl_typesupport_fastrtps_cpp/message_type_support.h>
#    include <rosidl_typesupport_introspection_c/identifier.h>
#    include <rosidl_typesupport_introspection_cpp/identifier.hpp>
#    include <rosidl_typesupport_introspection_cpp/field_types.hpp>
#    include <rosidl_typesupport_introspection_cpp/message_introspection.hpp>
#  else
#    define RCLCPP_DETAIL_HAS_FASTRTPS_TYPESUPPORT 0
#  endif
#else
#  define RCLCPP_DETAIL_HAS_FASTRTPS_TYPESUPPORT 0
#endif

namespace rclcpp
{
namespace detail
{

#if RCLCPP_DETAIL_HAS_FASTRTPS_TYPESUPPORT
template<typename ProtobufMessageT>
inline const rosidl_message_type_support_t &
get_protobuf_message_type_support_handle_fastrtps_cpp()
{
  static_assert(
    rclcpp::detail::is_protobuf_message<ProtobufMessageT>::value,
    "ProtobufMessageT must derive from google::protobuf::Message");

  static std::string full_name = ProtobufMessageT::GetDescriptor()->full_name();
  static std::string message_namespace = []() -> std::string {
    const auto pos = full_name.find_last_of('.');
    if (pos == std::string::npos) {
      return "protobuf";
    }
    std::string ns = full_name.substr(0, pos);
    // Convert proto package "a.b.c" -> "a::b::c" for DDS name generation.
    for (size_t i = 0; i < ns.size(); ++i) {
      if (ns[i] == '.') {
        ns[i] = ':';
        ns.insert(i + 1, 1, ':');
        ++i;
      }
    }
    return ns;
  }();
  static std::string message_name = []() -> std::string {
    const auto pos = full_name.find_last_of('.');
    if (pos == std::string::npos) {
      return full_name;
    }
    return full_name.substr(pos + 1);
  }();

  // Provide some stable names.
  static message_type_support_callbacks_t callbacks{};
  callbacks.message_namespace_ = message_namespace.c_str();
  callbacks.message_name_ = message_name.c_str();

  callbacks.cdr_serialize =
    [](const void * untyped_ros_message, eprosima::fastcdr::Cdr & cdr) -> bool
    {
      if (!untyped_ros_message) {
        return false;
      }
      const auto * msg = static_cast<const ProtobufMessageT *>(untyped_ros_message);
      const size_t n = static_cast<size_t>(msg->ByteSizeLong());
      if (n > static_cast<size_t>(std::numeric_limits<uint32_t>::max())) {
        return false;
      }

      // CDR encode as: uint32 length + raw bytes (sequence<uint8>).
      const uint32_t len = static_cast<uint32_t>(n);
      cdr << len;
      if (len > 0) {
        // Serialize protobuf directly into the CDR internal buffer to avoid an extra copy.
        // Since `len` is the element count of a uint8 sequence, no additional alignment is needed.
        char * dst = cdr.getCurrentPosition();
        if (!dst) {
          return false;
        }
        if (!msg->SerializeToArray(dst, static_cast<int>(len))) {
          return false;
        }
        if (!cdr.jump(len)) {
          return false;
        }
      }
      return true;
    };

  callbacks.cdr_deserialize =
    [](eprosima::fastcdr::Cdr & cdr, void * untyped_ros_message) -> bool  
    {
      if (!untyped_ros_message) {
        return false;
      }
      auto * msg = static_cast<ProtobufMessageT *>(untyped_ros_message);
      uint32_t len = 0;
      cdr >> len;
      if (len == 0) {
        msg->Clear();
        return true;
      }
      // Parse protobuf directly from the CDR internal buffer to avoid an extra copy.
      char * src = cdr.getCurrentPosition();
      if (!src) {
        return false;
      }
      if (!msg->ParseFromArray(src, static_cast<int>(len))) {
        return false;
      }
      return cdr.jump(len);
    };

  callbacks.get_serialized_size =
    [](const void * untyped_ros_message) -> uint32_t
    {
      if (!untyped_ros_message) {
        return 0u;
      }
      const auto * msg = static_cast<const ProtobufMessageT *>(untyped_ros_message);
      const size_t n = static_cast<size_t>(msg->ByteSizeLong());
      if (n > static_cast<size_t>(std::numeric_limits<uint32_t>::max())) {
        return 0u;
      }
      // CDR encodes sequences with a uint32 length prefix.
      return static_cast<uint32_t>(sizeof(uint32_t) + n);
    };

  callbacks.max_serialized_size =
    [](char & bounds_info) -> size_t
    {
      // Unbounded.
      bounds_info = ROSIDL_TYPESUPPORT_FASTRTPS_UNBOUNDED_TYPE;
      return 0u;
    };

  // Introspection used by rmw_fastrtps_* for type object registration.
  // Describe payload as: sequence<uint8> data
  static rosidl_typesupport_introspection_cpp::MessageMember member{};
  member.name_ = "data";
  member.type_id_ = rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT8;
  member.string_upper_bound_ = 0;
  member.members_ = nullptr;
  member.is_array_ = true;
  member.array_size_ = 0;
  member.is_upper_bound_ = false;
  member.offset_ = 0;
  member.default_value_ = nullptr;
  member.size_function = nullptr;
  member.get_const_function = nullptr;
  member.get_function = nullptr;
  member.fetch_function = nullptr;
  member.assign_function = nullptr;
  member.resize_function = nullptr;

  static rosidl_typesupport_introspection_cpp::MessageMembers introspection_members{};
  introspection_members.message_namespace_ = callbacks.message_namespace_;
  introspection_members.message_name_ = callbacks.message_name_;
  introspection_members.member_count_ = 1;
  introspection_members.members_ = &member;

  static rosidl_message_type_support_t fastrtps_ts{};
  fastrtps_ts.typesupport_identifier = rosidl_typesupport_fastrtps_cpp::typesupport_identifier;
  fastrtps_ts.data = &callbacks;
  fastrtps_ts.func = get_message_typesupport_handle_function;

  static rosidl_message_type_support_t introspection_cpp_ts{};
  introspection_cpp_ts.typesupport_identifier =
    rosidl_typesupport_introspection_cpp::typesupport_identifier;
  introspection_cpp_ts.data = &introspection_members;
  introspection_cpp_ts.func = get_message_typesupport_handle_function;

  struct TypeSupportBundle
  {
    rosidl_message_type_support_t fastrtps;
    rosidl_message_type_support_t introspection_cpp;
  };

  static TypeSupportBundle bundle{fastrtps_ts, introspection_cpp_ts};

  static rosidl_message_type_support_t top_level_ts{};
  top_level_ts.typesupport_identifier = "rclcpp_protobuf_typesupport";
  top_level_ts.data = &bundle;
  top_level_ts.func =
    [](const rosidl_message_type_support_t * handle, const char * id)
    -> const rosidl_message_type_support_t *
    {
      if (!handle || !id) {
        return nullptr;
      }
      auto * b = static_cast<const TypeSupportBundle *>(handle->data);
      if (!b) {
        return nullptr;
      }
      if (strcmp(id, rosidl_typesupport_fastrtps_cpp::typesupport_identifier) == 0) {
        return &b->fastrtps;
      }
      if (strcmp(id, rosidl_typesupport_introspection_cpp::typesupport_identifier) == 0) {
        return &b->introspection_cpp;
      }
      // Let introspection_c fall through (nullptr) to allow rmw_fastrtps to try cpp next.
      if (strcmp(id, rosidl_typesupport_introspection_c__identifier) == 0) {
        return nullptr;
      }
      return nullptr;
    };

  return top_level_ts;
}
#else
template<typename ProtobufMessageT>
inline const rosidl_message_type_support_t &
get_protobuf_message_type_support_handle_fastrtps_cpp()
{
  static_assert(
    sizeof(ProtobufMessageT) == 0,
    "FastRTPS typesupport headers not available; cannot support protobuf messages");
  static rosidl_message_type_support_t dummy{};
  return dummy;
}
#endif

}  // namespace detail
}  // namespace rclcpp

#endif  // RCLCPP__DETAIL__PROTOBUF_TYPESUPPORT_FASTRTPS_HPP_
