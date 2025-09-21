#!/bin/sh

SCRIPT_DIR=$(dirname "$0")

pacman -Syu glm glfw stb vulkan-tools glslc gldlang
pacman -Syu cmake

if [ -e $SCRIPT_DIR/../../tinyobjloader ]
then
  echo "tinyobjloader already exists!"
else
  git clone --depth 1 git@github.com:tinyobjloader/tinyobjloader.git $SCRIPT_DIR/../../tinyobjloader
fi

if [ -e $SCRIPT_DIR/../../slang ]
then
    echo "Slang already exists!"
else
    mkdir $SCRIPT_DIR/../../slang
    cd $SCRIPT_DIR/../../slang
    git clone https://github.com/shader-slang/slang --recursive .
    cmake --preset default
    cmake --build build --preset release
    cmake --install build --prefix ./install
fi    
    



