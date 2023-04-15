#!/bin/bash

mkdir -p build


export CC=clang
export CXX=clang++

export CXXFLAGS="-isystem ./include"

cmake -S . -B ./build 
cmake --build ./build



exit 0
