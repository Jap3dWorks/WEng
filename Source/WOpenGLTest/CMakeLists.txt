cmake_minimum_required (VERSION 3.26.2)

add_executable(WOpenGLTest Source/WOpenGLTest.cpp)

message(STATUS "WOpenGLTest Standalone")
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)
find_package(glm REQUIRED)
find_package(STB REQUIRED)

target_include_directories(WOpenGLTest PUBLIC Include Source ${STB_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})
target_link_libraries(
    WOpenGLTest
    OpenGL::GL 
    GLUT::GLUT 
    GLEW::GLEW 
    SDL2::SDL2main 
    SDL2::SDL2
    glm 
)

install(TARGETS WOpenGLTest DESTINATION bin)
