@echo off

title Environment

:choice
SET /P AREYOUSURE=Emscripten or Desktop env (E/[D])?
IF /I "%AREYOUSURE%" EQU "E" GOTO YES
IF /I "%AREYOUSURE%" NEQ "E" GOTO NO

:YES
set PATH=%PATH%;%~dp0\..\emsdk\upstream\emscripten;%~dp0\..\emsdk\upstream\emscripten\emrun;%~dp0\..\emsdk\upstream\bin;C:\msys64\mingw64\bin
prompt (WEng Emscripten) $P$G
set EMSCRIPTENENV=1
GOTO END

:NO
set PATH=%PATH%;C:\msys64\mingw64\bin
prompt (WEng Desktop) $P$G
GOTO END

:END


cmd /k
