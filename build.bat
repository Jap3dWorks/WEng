@echo off

rmdir build /s /q
mkdir build

set CC=emcc
set CXX=em++

emcmake cmake -S . -B ./build
:: cmake --build ./build

pause
exit 0

