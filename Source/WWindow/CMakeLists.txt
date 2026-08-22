set(GLFW_BUILD_WAYLAND ON CACHE BOOL "Build GLFW with Wayland support")
set(GLFW_BUILD_X11 OFF CACHE BOOL "")

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.5.1
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(glfw)

add_library(
    WWindow 
    SHARED
        Source/WWindow.cpp
        Source/WInput.cpp
        Source/Vulkan.cpp
    )

set_target_properties(
    WWindow
    PROPERTIES
        CXX_STANDARD 23
)

add_dependencies(WWindow ${GENERATE_CODE_TARGET})

target_include_directories(
    WWindow 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PublicGenerated>
    PRIVATE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PrivateGenerated>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source>
)

target_link_libraries(
    WWindow
    PRIVATE
        WObjects
        WCore
        WInterfaces
        glfw
)

# install(
#     TARGETS WWindow
#     EXPORT WWindowTargets
#     LIBRARY DESTINATION lib64
#     ARCHIVE DESTINATION lib64
#     RUNTIME DESTINATION bin
#     INCLUDES DESTINATION include
# )

# # Generate and install config files
# install(
#     EXPORT WWindowTargets
#     FILE WWindowConfig.cmake
#     NAMESPACE WWindow::
#     DESTINATION lib/cmake/WWindow
# )

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
#     WWindowConfigVersion.cmake
#     VERSION ${PROJECT_VERSION}
#     COMPATIBILITY SameMajorVersion
# )

# # install(
# #     FILES ${CMAKE_CURRENT_BINARY_DIR}/WWindowConfigVersion.cmake
# #     DESTINATION lib64/cmake/WWindow
# # )

