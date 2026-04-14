#!/usr/bin/env bash

ROS2_PROTOBUF_HOME_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/.." && pwd )"

display=""
if [ -z ${DISPLAY} ];then
    display=":1"
else
    display="${DISPLAY}"
fi

local_host="$(hostname)" 
user="${USER}"
uid="$(id -u)"
group="$(id -g -n)"
gid="$(id -g)"


echo "stop and rm docker" 
docker stop ros2_humble_protobuf > /dev/null
docker rm -v -f ros2_humble_protobuf > /dev/null

echo "start docker"
docker run -it -d \
--privileged=true \
--name ros2_humble_protobuf \
-e DISPLAY=$display \
-e DOCKER_USER="${user}" \
-e USER="${user}" \
-e DOCKER_USER_ID="${uid}" \
-e DOCKER_GRP="${group}" \
-e DOCKER_GRP_ID="${gid}" \
-e XDG_RUNTIME_DIR=$XDG_RUNTIME_DIR \
-v ${ROS2_PROTOBUF_HOME_DIR}/example:/root/work/ros2_humble/src/example \
-v ${ROS2_PROTOBUF_HOME_DIR}/rclcpp:/root/work/ros2_humble/src/ros2/rclcpp \
-v ${XDG_RUNTIME_DIR}:${XDG_RUNTIME_DIR} \
--network host \
ros2-humble:v1.3