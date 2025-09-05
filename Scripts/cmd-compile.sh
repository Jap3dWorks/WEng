#!/usr/bin/sh

SRC_DIR=$(dirname "$0")
cd ${SRC_DIR}/..

source ${SRC_DIR}/source-environment.sh

export slang_DIR=$SRC_DIR/../../slang/install/cmake
export WENG_WLOG_ENABLE=1

wcli Build -t Release -a x86_64

