cmake_minimum_required (VERSION 3.26.2)

project(WSandBox)

if (EMSCRIPTEN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lGL -lGLU -lglut -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lGL -lGLU -lglut -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif ()

# find_package(glm REQUIRED)

# include_directories(include ${GLM_INCLUDE_DIRS})

add_executable(WSandBox src/WSandBox.cpp)
# target_link_libraries(WSandBox glm)

