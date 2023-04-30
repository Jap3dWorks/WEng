#!/bin/bash

mkdir -p build

export CC=clang
export CXX=clang++

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

emcmake cmake  -S . -B ./build 
# cmake --build ./build

exit 0
