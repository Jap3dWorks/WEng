#!/bin/sh

SCRIPT_DIR=$(dirname "$0")

dnf install glm-devel glfw-devel stb-devel -y
dnf install vulkan-devel vulkan-tools vulkan-validation-layers libXxf86vm libXxf86vm-devel glslc -y

if [ -e $SCRIPT_DIR/../../tinyobjloader ]
then
  echo "tinyobjloader already exists!"
else
  git clone --depth 1 git@github.com:tinyobjloader/tinyobjloader.git $SCRIPT_DIR/../../tinyobjloader
fi
