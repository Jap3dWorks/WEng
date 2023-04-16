#!/bin/bash

mkdir -p build


export CC=clang
export CXX=clang++

# https://clang.llvm.org/docs/JSONCompilationDatabase.html
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# cmake  -DCMAKE_CXX_FLAGS="-isystem includes" -CMAKE_C_COMPILER="clang" -CMAKE_CXX_COMPILER="clang++" -CMAKE_COMPILE_COMMANDS 1 -S . -B ./build 
cmake  -DCMAKE_CXX_FLAGS="-isystem $SCRIPT_DIR/includes" -S . -B ./build 
cmake --build ./build

rm compile_commands.json
ln -s $SCRIPT_DIR/build/compile_commands.json $SCRIPT_DIR/compile_commands.json

exit 0
