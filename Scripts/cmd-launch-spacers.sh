#!/bin/sh

SRC_DIR=$(dirname "$0")

cd $SRC_DIR/../Install/Release

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib

./bin/WSpacers

