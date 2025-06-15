#!/bin/sh

SRC_DIR=$(dirname "$0")
cd $SRC_DIR/..

source ${SRC_DIR}/source-environment.sh

./Install/Linux-x86_64_Release_Standalone/bin/WVulkanTest

