@echo off
mkdir build


set CC=clang
set CXX=clang++

cmake -S . -B ./build
cmake --build ./build

exit 0

