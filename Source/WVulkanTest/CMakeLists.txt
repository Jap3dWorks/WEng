# cmake_minimum_required (VERSION 3.20)

add_executable(WVulkanTest Source/WVulkanTest.cpp)

find_package(glm REQUIRED)
find_package(STB REQUIRED)
find_package(GLFW REQUIRED)
find_package(Vulkan REQUIRED)

target_include_directories(
    WVulkanTest PUBLIC 
    Include 
    Source 
    ${STB_INCLUDE_DIRS} 
    ${GLM_INCLUDE_DIRS} 
    ${GLFW_INCLUDE_DIRS}
    ${VULKAN_INCLUDE_DIRS}
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
    # ${CMAKE_CURRENT_SOURCE_DIR}/Assets 
    DESTINATION bin/WVulkanTest_Content
)

