cmake_minimum_required (VERSION 3.26.2)

add_executable(WSandBox Source/WSandBox.cpp)

if (EMSCRIPTEN)
    message(STATUS "WSandBox EMSCRIPTEN")
    target_compile_definitions(WSandBox PUBLIC MODULE_NAME=WSANDBOX)
    target_compile_definitions(WSandBox PUBLIC WSANDBOX_API)

    target_compile_options(WSandBox BEFORE PRIVATE -lSDL -sUSE_SDL=2 -sUSE_SDL_IMAGE=2 -sSDL2_IMAGE_FORMATS=["bmp","png","xpm"] -sUSE_SDL_NET=2)
    target_link_options(WSandBox BEFORE PRIVATE -lSDL -sUSE_SDL=2 -sUSE_SDL_IMAGE=2 -sSDL2_IMAGE_FORMATS=["bmp","png","xpm"] -sUSE_SDL_NET=2)
    
    target_compile_definitions(WSandBox PUBLIC GLM_FORCE_PURE)
    target_include_directories(WSandBox PUBLIC include thirdParty/glm)
else()
    message(STATUS "WSandBox Standalone")
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(SDL2 REQUIRED)
    find_package(glm REQUIRED)
    
    target_include_directories(WSandBox PUBLIC include ${GLM_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})
    target_link_libraries(
        WSandBox 
        OpenGL::GL 
        OpenGL::GLU 
        GLUT::GLUT 
        GLEW::GLEW 
        SDL2::SDL2main 
        SDL2::SDL2
        glm 
    )

endif ()
