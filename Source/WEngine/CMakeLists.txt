cmake_minimum_required (VERSION 3.26.2)

project(WEng)

if (NOT EMSCRIPTEN)
find_package(glm REQUIRED)
include_directories(include ${GLM_INCLUDE_DIRS})
else()
include_directories(include)
endif()

add_executable(WEng Source/WEngine.cpp)
if (NOT EMSCRIPTEN)
target_link_libraries(WEng glm)
endif()

