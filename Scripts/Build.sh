#!/bin/bash

BUILD_PATH = "./Build"
mkdir -p $BUILD_PATH

export CC=clang
export CXX=clang++

export BUILD_TYPE="Debug"

OPTIONS = ("Debug", "Release", "Quit")
select opt in "${OPTIONS[@]"
do
    case $opt in
        "Debug")
            BUILD_TYPE="Debug"
            break
            ;;
        "Release")
            BUILD_TYPE="Release"
            break
            ;;
        "Quit")
            exit 0
            ;;
        *) echo "invalid option $REPLY";;
    esac
done

exit 0

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

emcmake cmake  -S . -B ./Build 
# cmake --build ./build


exit 0
