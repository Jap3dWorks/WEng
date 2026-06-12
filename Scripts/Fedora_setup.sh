#!/bin/sh

SCRIPT_DIR=$(dirname "$0")

dnf install glm-devel glfw-devel stb-devel -y
dnf install vulkan-devel vulkan-tools vulkan-validation-layers libXxf86vm libXxf86vm-devel glslc glslang -y
.dnf install catch2-devel
dnf install cmake -y

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
    cmake --build --preset release
    cmake --install build --prefix ./install
fi    
    
