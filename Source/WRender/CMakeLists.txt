
add_library(
    WRender 
    SHARED 
        CompileGenerated/CompileGenerated.cpp
        Source/WVkRender.cpp
        Source/WVulkan.cpp
	    Source/WVkRenderCommandPoolRAII.cpp
        Source/WVkAssetRenderDataRAII.cpp
        Source/WVkPostprocessPipelinesRAII.cpp
        Source/WVkSurfaceRAII.cpp
        Source/WVkTexture.cpp
        Source/WVkDescriptor.cpp
        Source/WVkBuffer.cpp
        Source/WVkMesh.cpp
        Source/WVkImage.cpp
        Source/WVkPipeline.cpp
        Source/WVkSwapChain.cpp
    )

set_target_properties(
    WRender
    PROPERTIES
        CXX_STANDARD 23
)
    

find_package(glm REQUIRED)
find_package(STB REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

target_include_directories(
    WRender 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PublicGenerated>
    PRIVATE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PrivateGenerated>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source>
)

target_link_libraries(
    WRender
    PRIVATE
        WEngineObjects
        WCore
        WEngineInterfaces
        glm::glm
        glfw
        WWindow
        Vulkan::Vulkan
        dl
        pthread
        # X11
        # Xxf86vm 
        # Xrandr 
        # Xi
)

include(ShaderUtils)

add_slang_shader_target(
    WRender_AllShaders
    SHADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content/Shaders
)

# compile shaders first
add_dependencies(WRender WRender_AllShaders)

install(
    TARGETS WRender
    EXPORT WRenderTargets
    LIBRARY DESTINATION lib64
    ARCHIVE DESTINATION lib64
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Generate and install config files
install(
    EXPORT WRenderTargets
    FILE WRenderConfig.cmake
    NAMESPACE WRender::
    DESTINATION lib/cmake/WRender
)

# Install public headers
install(
    DIRECTORY Include/
    DESTINATION include
    FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.h"
)
    
install(
    DIRECTORY PublicGenerated/
    DESTINATION include
    FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.h"
)

# # Version config file
# include(CMakePackageConfigHelpers)
# write_basic_package_version_file(
#     WRenderConfigVersion.cmake
#     VERSION ${PROJECT_VERSION}
#     COMPATIBILITY SameMajorVersion
# )

# install(
#     FILES ${CMAKE_CURRENT_BINARY_DIR}/WRenderConfigVersion.cmake
#     DESTINATION lib64/cmake/WRender
# )

install(
    DIRECTORY
      ${CMAKE_CURRENT_SOURCE_DIR}/Content
    DESTINATION
      .
)

