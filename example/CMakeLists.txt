cmake_minimum_required(VERSION 3.8)
project(example)

find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/BigBytes.msg"
)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(nav_msgs REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rosidl_cmake REQUIRED)
find_package(std_msgs REQUIRED)
find_package(Protobuf QUIET)

include_directories(include)

function(example_latency_ros2msg_nested_executable target)
  add_executable(${target} src/topics/${target}.cpp)
  ament_target_dependencies(${target}
    "nav_msgs"
    "rclcpp")
  install(TARGETS ${target} DESTINATION lib/${PROJECT_NAME})
endfunction()

add_executable(publisher_node src/publisher.cpp)
ament_target_dependencies(publisher_node rclcpp std_msgs)

add_executable(subscriber_node src/subscriber.cpp)
ament_target_dependencies(subscriber_node rclcpp std_msgs)

install(TARGETS publisher_node subscriber_node
  DESTINATION lib/${PROJECT_NAME})

rosidl_get_typesupport_target(cpp_typesupport_target "${PROJECT_NAME}" "rosidl_typesupport_cpp")
rosidl_get_typesupport_target(fastrtps_typesupport_target "${PROJECT_NAME}" "rosidl_typesupport_fastrtps_cpp")

add_executable(latency_ros2msg_big_talker src/topics/latency_ros2msg_big_talker.cpp)
ament_target_dependencies(latency_ros2msg_big_talker rclcpp)
# With default --as-needed, the Fast DDS typesupport .so may be dropped from DT_NEEDED
# (only loaded via dlopen at runtime). Force it linked so the loader resolves it under
# install/<pkg>/lib (RPATH $ORIGIN/.. from lib/<pkg>/).
target_link_libraries(latency_ros2msg_big_talker
  "-Wl,--no-as-needed"
  "${cpp_typesupport_target}"
  "${fastrtps_typesupport_target}"
  "-Wl,--as-needed")
set_target_properties(latency_ros2msg_big_talker PROPERTIES INSTALL_RPATH "\$ORIGIN/..")
install(TARGETS latency_ros2msg_big_talker DESTINATION lib/${PROJECT_NAME})

add_executable(latency_ros2msg_big_listener src/topics/latency_ros2msg_big_listener.cpp)
ament_target_dependencies(latency_ros2msg_big_listener rclcpp)
target_link_libraries(latency_ros2msg_big_listener
  "-Wl,--no-as-needed"
  "${cpp_typesupport_target}"
  "${fastrtps_typesupport_target}"
  "-Wl,--as-needed")
set_target_properties(latency_ros2msg_big_listener PROPERTIES INSTALL_RPATH "\$ORIGIN/..")
install(TARGETS latency_ros2msg_big_listener DESTINATION lib/${PROJECT_NAME})

example_latency_ros2msg_nested_executable(latency_ros2msg_nested_talker)
example_latency_ros2msg_nested_executable(latency_ros2msg_nested_listener)

if(Protobuf_FOUND)
  set(PROTO_FILE ${CMAKE_CURRENT_SOURCE_DIR}/proto/simple.proto)
  protobuf_generate_cpp(EXAMPLE_PROTO_SRCS EXAMPLE_PROTO_HDRS ${PROTO_FILE})

  add_executable(protobuf_talker
    src/topics/protobuf_talker.cpp
    ${EXAMPLE_PROTO_SRCS})
  target_include_directories(protobuf_talker PRIVATE
    ${Protobuf_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR})
  target_link_libraries(protobuf_talker ${Protobuf_LIBRARIES})
  ament_target_dependencies(protobuf_talker "rclcpp")
  install(TARGETS protobuf_talker DESTINATION lib/${PROJECT_NAME})

  add_executable(protobuf_listener
    src/topics/protobuf_listener.cpp
    ${EXAMPLE_PROTO_SRCS})
  target_include_directories(protobuf_listener PRIVATE
    ${Protobuf_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR})
  target_link_libraries(protobuf_listener ${Protobuf_LIBRARIES})
  ament_target_dependencies(protobuf_listener "rclcpp")
  install(TARGETS protobuf_listener DESTINATION lib/${PROJECT_NAME})

  add_executable(latency_protobuf_big_talker
    src/topics/latency_protobuf_big_talker.cpp ${EXAMPLE_PROTO_SRCS})
  target_include_directories(latency_protobuf_big_talker PRIVATE
    ${Protobuf_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
  target_link_libraries(latency_protobuf_big_talker ${Protobuf_LIBRARIES})
  ament_target_dependencies(latency_protobuf_big_talker "rclcpp")
  install(TARGETS latency_protobuf_big_talker DESTINATION lib/${PROJECT_NAME})

  add_executable(latency_protobuf_big_listener
    src/topics/latency_protobuf_big_listener.cpp ${EXAMPLE_PROTO_SRCS})
  target_include_directories(latency_protobuf_big_listener PRIVATE
    ${Protobuf_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
  target_link_libraries(latency_protobuf_big_listener ${Protobuf_LIBRARIES})
  ament_target_dependencies(latency_protobuf_big_listener "rclcpp")
  install(TARGETS latency_protobuf_big_listener DESTINATION lib/${PROJECT_NAME})

  add_executable(latency_protobuf_nested_talker
    src/topics/latency_protobuf_nested_talker.cpp ${EXAMPLE_PROTO_SRCS})
  target_include_directories(latency_protobuf_nested_talker PRIVATE
    ${Protobuf_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
  target_link_libraries(latency_protobuf_nested_talker ${Protobuf_LIBRARIES})
  ament_target_dependencies(latency_protobuf_nested_talker "rclcpp")
  install(TARGETS latency_protobuf_nested_talker DESTINATION lib/${PROJECT_NAME})

  add_executable(latency_protobuf_nested_listener
    src/topics/latency_protobuf_nested_listener.cpp ${EXAMPLE_PROTO_SRCS})
  target_include_directories(latency_protobuf_nested_listener PRIVATE
    ${Protobuf_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
  target_link_libraries(latency_protobuf_nested_listener ${Protobuf_LIBRARIES})
  ament_target_dependencies(latency_protobuf_nested_listener "rclcpp")
  install(TARGETS latency_protobuf_nested_listener DESTINATION lib/${PROJECT_NAME})
else()
  message(WARNING "Protobuf not found; skipping protobuf_* and latency_protobuf_* targets")
endif()

ament_export_dependencies(rosidl_default_runtime)

ament_package()
