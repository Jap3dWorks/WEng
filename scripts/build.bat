@echo off

set BUILDTYPE=Debug
:choice
SET /P AREYOUSURE=Release or Debug (R/[D])?
IF /I "%AREYOUSURE%" EQU "R" GOTO RELEASE
IF /I "%AREYOUSURE%" NEQ "R" GOTO DEBUG

:RELEASE
set BUILDTYPE=Release
GOTO END

:DEBUG
GOTO END
:END

rmdir build /s /q
mkdir build

set CC=clang
set CXX=clang++

:: -DCMAKE_GENERATOR_PLATFORM=Win64
if "%EMSCRIPTENENV%"=="" (
    set glm_DIR=C:/msys64/mingw64/lib/cmake/glm
    call emcmake cmake -G "MinGW Makefiles" -Dglm_DIR=%glm_DIR% -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_BUILD_TYPE=%BUILDTYPE% -S . -B .\build
) else (
    call emcmake cmake -G "MinGW Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_BUILD_TYPE=Debug -S . -B .\build
)

:choice
SET /P AREYOUSURE=Build cmake files? (Y/[N])?
IF /I "%AREYOUSURE%" EQU "Y" GOTO YES
IF /I "%AREYOUSURE%" NEQ "Y" GOTO NO

:YES
cmake --build ./build
GOTO END
:NO
GOTO END
:END


