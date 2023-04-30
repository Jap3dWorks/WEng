@echo off

rmdir build /s /q
mkdir build

set CC=emcc
set CXX=em++

emcmake cmake -S . -B ./build
<<<<<<< Updated upstream
:: cmake --build ./build
=======
cmake --build ./build
>>>>>>> Stashed changes

pause
exit 0

