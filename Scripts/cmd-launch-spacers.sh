#!/bin/sh

SRC_DIR=$(dirname "$0")

cd $SRC_DIR/../Install/Linux_x86_64_Release_Standalone

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib

./bin/WSpacers

