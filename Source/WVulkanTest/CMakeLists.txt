
add_executable(WVulkanTest Source/WVulkanTest.cpp)

find_package(glm REQUIRED)
find_package(STB REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(tinyobjloader REQUIRED)

target_include_directories(
    WVulkanTest 
    PUBLIC 
        Include  
        ${STB_INCLUDE_DIRS} 
        ${GLM_INCLUDE_DIRS} 
        ${GLFW_INCLUDE_DIRS}
        ${VULKAN_INCLUDE_DIRS}
        ${tinyobjloader_INCLUDE_DIRS}
    PRIVATE
        Source
)

target_link_libraries(
    WVulkanTest
        vulkan
        glfw
        dl pthread X11 Xxf86vm Xrandr Xi
)

install(TARGETS WVulkanTest DESTINATION bin)
install(
    DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/Shaders
    ${CMAKE_CURRENT_SOURCE_DIR}/Assets
    DESTINATION bin/WVulkanTest_Content
)

# Vulkan Compute Test

add_executable(WVulkanComputeTest Source/WVulkanComputeTest.cpp)

target_include_directories(
    WVulkanComputeTest PUBLIC 
    Include 
    Source 
    ${STB_INCLUDE_DIRS} 
    ${GLM_INCLUDE_DIRS} 
    ${GLFW_INCLUDE_DIRS}
    ${VULKAN_INCLUDE_DIRS}
    ${tinyobjloader_INCLUDE_DIRS}
)
target_link_libraries(
    WVulkanComputeTest
    vulkan
    glfw
    dl pthread X11 Xxf86vm Xrandr Xi
)
install(TARGETS WVulkanComputeTest DESTINATION bin)
install(
    DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/Shaders
    ${CMAKE_CURRENT_SOURCE_DIR}/Assets
    DESTINATION bin/WVulkanComputeTest_Content
)
