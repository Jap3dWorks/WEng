add_library(
    WRender 
    SHARED 
        Compile/Compile.cpp
        Source/WRender.cpp
        Source/WRenderPipeline.cpp
        Source/WShader.cpp
        Source/WVulkan.cpp
    )

find_package(glm REQUIRED)
find_package(STB REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

target_include_directories(
    WRender 
    PUBLIC 
        Include
    PRIVATE 
        Intermediate
        WCore::WCore
        ${STB_INCLUDE_DIRS} 
        ${GLM_INCLUDE_DIRS} 
        ${GLFW_INCLUDE_DIRS}
        ${VULKAN_INCLUDE_DIRS}
        Source
)

target_link_libraries(
    WRender
    PRIVATE
        WCore::WCore
        glm::glm
        glfw
        Vulkan::Vulkan
        dl
        pthread
        X11
        Xxf86vm 
        Xrandr 
        Xi
)

add_library(WRender::WRender ALIAS WRender)

install(
    TARGETS WRender
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)
