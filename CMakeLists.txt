
cmake_minimum_required (VERSION 3.26.2)

project (StorageResolver)


# Generates a `compile_commands.json` that can be used for autocompletion
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Enable/Disable output of compile commands during generation.")

# Use a specific C/C++ compiler, e.g. llvm-clang on macOS (so we can use clangd)
set(CMAKE_C_COMPILER   "clang" CACHE STRING "C compiler" FORCE)
set(CMAKE_CXX_COMPILER "clang++" CACHE STRING "C++ compiler" FORCE)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# add_subdirectory(include)
# add_executable(src/main.cpp)

include_directories(include)

add_executable(StorageResolver src/main.cpp)

