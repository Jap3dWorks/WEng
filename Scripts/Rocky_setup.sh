#!/bin/sh

SCRIPT_DIR=$(dirname "$0")

dnf install glm-devel glfw-devel -y
dnf install vulkan-devel vulkan-tools libXxf86vm -y

if [ -e $SCRIPT_DIR/../../tinyobjloader ]
then
  echo "tinyobjloader already exists!"
else
  git clone --depth 1 git@github.com:tinyobjloader/tinyobjloader.git $SCRIPT_DIR/../../tinyobjloader
fi
