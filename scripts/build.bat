@echo off

rmdir build /s /q
mkdir build

set CC=emcc
set CXX=em++

emcmake cmake -Dglm_DIR=%glm_DIR% -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_BUILD_TYPE=Debug -S . -B .\build
:: cmake --build ./build

pause
exit 0

