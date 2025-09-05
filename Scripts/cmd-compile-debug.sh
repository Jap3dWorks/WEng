#!/usr/bin/sh

SRC_DIR=$(dirname "$0")
cd ${SRC_DIR}/..

source $SRC_DIR/source-environment.sh

export WUNITTEST=1
export WENG_WLOG_ENABLE=1

export slang_DIR=$SRC_DIR/../../slang/install/cmake

wcli Build -t Debug -a x86_64


