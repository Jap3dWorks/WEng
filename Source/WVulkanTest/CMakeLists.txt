cmake_minimum_required (VERSION 3.26.2)

add_executable(WVulkanTest Source/WVulkanTest.cpp)

# find_package(OpenGL REQUIRED)
# find_package(GLUT REQUIRED)
# find_package(GLEW REQUIRED)
# find_package(SDL2 REQUIRED)
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
    glm 
    vulkan
    glfw
    dl pthread X11 Xxf86vm Xrandr Xi
)

install(TARGETS WVulkanTest DESTINATION bin)