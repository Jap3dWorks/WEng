cmake_minimum_required (VERSION 3.26.2)

# project(WSandBox)
add_executable(WSandBox src/WSandBox.cpp)

if (EMSCRIPTEN)
    target_compile_definitions(WSandBox PUBLIC MODULE_NAME=WSANDBOX)
    target_compile_definitions(WSandBox PUBLIC WSANDBOX_API)

    target_compile_options(WSandBox BEFORE PRIVATE -sUSE_SDL=2 -sUSE_SDL_IMAGE=2 -sSDL2_IMAGE_FORMATS=["bmp","png","xpm"] -sUSE_SDL_NET=2)
    target_link_options(WSandBox BEFORE PRIVATE -sUSE_SDL=2 -sUSE_SDL_IMAGE=2 -sSDL2_IMAGE_FORMATS=["bmp","png","xpm"] -sUSE_SDL_NET=2)
    target_include_directories(WSandBox PUBLIC include)
else()
    find_package(glm REQUIRED)
    target_include_directories(WSandBox PUBLIC include ${GLM_INCLUDE_DIRS})
endif ()

if(NOT EMSCRIPTEN)
    target_link_libraries(WSandBox glm)
endif()
