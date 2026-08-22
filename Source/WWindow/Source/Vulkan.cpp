#include "WWindow/Vulkan.hpp"

#include <GLFW/glfw3.h>


char const ** wdw::vulkan::GetRequiredInstanceExtensions(std::uint32_t & out_extension_count) {
    return glfwGetRequiredInstanceExtensions(&out_extension_count);
}

