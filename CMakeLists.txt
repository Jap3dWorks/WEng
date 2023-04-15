
cmake_minimum_required (VERSION 3.26.3)

project (HelixTestProject)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# add_subdirectory(include)

# add_executable(src/main.cpp)

include_directories(include)

add_executable(HelixTestProject src/main.cpp)