#!/usr/bin/sh

SRC_DIR=$(dirname "$0")
cd ${SRC_DIR}/..

source $SRC_DIR/source-environment.sh

wcli Build -t Debug -a x86_64
