
cmake_minimum_required (VERSION 3.26.2)

project (WebAssemblyProject)


# Generates a `compile_commands.json` that can be used for autocompletion
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Enable/Disable output of compile commands during generation.")

# Use a specific C/C++ compiler, e.g. llvm-clang on macOS (so we can use clangd)
# set(CMAKE_C_COMPILER   "emcc" CACHE STRING "C compiler" FORCE)
# set(CMAKE_CXX_COMPILER "em++" CACHE STRING "C++ compiler" FORCE)

if (EMSCRIPTEN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif ()

# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# add_subdirectory(include)
# add_executable(src/main.cpp)

# find_path("emscripten/emscripten.h" REQUIRED)

include_directories(include)
# include_directories("${FOO_INCLUDE_DIR}")
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../emsdk/upstream/emscripten/cache/sysroot/include)


add_executable(WebAssemblyProject src/main.cpp)

