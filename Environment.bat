@echo off

@REM title Environment

@REM :choice
@REM SET /P AREYOUSURE=Emscripten or Desktop env (E/[D])?
@REM IF /I "%AREYOUSURE%" EQU "E" GOTO YES
@REM IF /I "%AREYOUSURE%" NEQ "E" GOTO NO

@REM :YES
@REM set PATH=%PATH%;%~dp0\..\emsdk\upstream\emscripten;%~dp0\..\emsdk\upstream\emscripten\emrun;%~dp0\..\emsdk\upstream\bin;C:\msys64\mingw64\bin
@REM prompt (WEng Emscripten) $P$G
@REM set EMSCRIPTENENV=1
@REM GOTO END

@REM :NO
@REM set PATH=%PATH%;C:\msys64\mingw64\bin
@REM prompt (WEng Desktop) $P$G
@REM GOTO END

@REM :END

set PATH=%PATH%;./Scripts

echo "Environment set up for wcli"

cmd /k

