#!/usr/bin/env bash

xhost +local:root 1>/dev/null 2>&1
docker exec \
    -u root \
    -it ros2_humble_protobuf \
    /bin/bash
xhost -local:root 1>/dev/null 2>&1