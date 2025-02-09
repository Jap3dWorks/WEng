#!/bin/sh

SCRIPT_DIR=$(dirname "$0")

pacman -Syu glm glfw stb vulkan-tools

if [ -e $SCRIPT_DIR/../../tinyobjloader ]
then
  echo "tinyobjloader already exists!"
else
  git clone --depth 1 git@github.com:tinyobjloader/tinyobjloader.git $SCRIPT_DIR/../../tinyobjloader
fi
