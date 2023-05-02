@echo off

rmdir build /s /q
mkdir build

set CC=emcc
set CXX=em++

@REM set CMAKE_PREFIX_PATH=C:/msys64/ucrt64
@REM set CMAKE_MODULE_PATH=C:/msys64/ucrt64
@REM set CMAKE_EXPORT_COMPILE_COMMANDS=ON

set glm_DIR=C:/msys64/ucrt64/lib/cmake/glm

:: emcmake cmake -Dglm_DIR=%glm_DIR% -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_BUILD_TYPE=Debug -S . -B .\build
:: emcmake cmake -Dglm_DIR=%glm_DIR% -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_GENERATOR_PLATFORM=Win64 -S . -B .\build
emcmake cmake -Dglm_DIR=%glm_DIR% -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_BUILD_TYPE=Debug -S . -B .\build
:: cmake --build ./build

pause
exit 0

