@echo off

set BUILD_PATH=.\build

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

set SYSTEM=win32
set ARCH=x64
:: or x86

if "%EMSCRIPTENENV%"=="" (
    set BUILD_PATH=%BUILD_PATH%\%SYSTEM%_%ARCH%_%BUILDTYPE%_Standalone
) else (
    set BUILD_PATH=%BUILD_PATH%\%SYSTEM%_%ARCH%_%BUILDTYPE%_Emscripten
)

:: rmdir build /s /q
mkdir %BUILD_PATH%

set CC=clang
set CXX=clang++

echo "Build PATH %BUILD_PATH%"

:: -DCMAKE_GENERATOR_PLATFORM=Win64
if "%EMSCRIPTENENV%"=="" (
    set glm_DIR=C:/msys64/mingw64/lib/cmake/glm
    call cmake -G "MinGW Makefiles" -Dglm_DIR=%glm_DIR% -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_BUILD_TYPE=%BUILDTYPE% -S . -B %BUILD_PATH%
) else (
    call emcmake cmake -G "MinGW Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=OFF -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_BUILD_TYPE=%BUILDTYPE% -S . -B %BUILD_PATH%
)

:choice
SET /P BUILDFILES=Build cmake files? (Y/[N])?
IF /I "%BUILDFILES%" EQU "Y" GOTO YES
IF /I "%BUILDFILES%" NEQ "Y" GOTO NO

:YES
cmake --build %BUILD_PATH%
GOTO ENDB
:NO
GOTO ENDB
:ENDB


