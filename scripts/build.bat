@echo off

rmdir build /s /q
mkdir build

set CC=emcc
set CXX=em++

emcmake cmake -Dglm_DIR=%glm_DIR% -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -S . -B .\build
:: cmake --build ./build

pause
exit 0

