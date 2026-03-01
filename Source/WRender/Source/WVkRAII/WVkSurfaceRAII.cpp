#include "WVulkan/WVkRAII/WVkSurfaceRAII.hpp"

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif

#include <GLFW/glfw3.h>

WVkSurfaceRAII::WVkSurfaceRAII(const VkInstance & in_instance,
                               GLFWwindow * in_window) :
    vk_instance_(in_instance)
{
    WVulkan::ExecVkProcChecked(
        glfwCreateWindowSurface,
        "Failed to create window surface!",
        vk_instance_,
        in_window,
        nullptr,
        &vk_surface_
        );
}
