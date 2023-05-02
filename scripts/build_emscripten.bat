@echo off

rmdir build /s /q
mkdir build

set CC=emcc
set CXX=em++

set glm_DIR=C:/msys64/mingw64/lib/cmake/glm
:: emcmake cmake -Dglm_DIR=%glm_DIR% -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_BUILD_TYPE=Debug -S . -B .\build
:: emcmake cmake -Dglm_DIR=%glm_DIR% -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_GENERATOR_PLATFORM=Win64 -S . -B .\build
:: emcmake cmake -G "Visual Studio 17 2022" -A x64 -Dglm_DIR=%glm_DIR% -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_BUILD_TYPE=Debug -S . -B .\build
emcmake cmake -G "MinGW Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_BUILD_TYPE=Debug -S . -B .\build
:: cmake --build ./build

pause
exit 0

