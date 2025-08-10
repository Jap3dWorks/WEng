#pragma once

#include "WCore/WCore.hpp"

#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

struct WWindowInfo
{

    std::string title {"WEngine"};
    uint32_t width {800};
    uint32_t height {600};

    // GLFWframebuffersizefun framebuffer_size_callback {nullptr};
    void * user_pointer {nullptr};

    GLFWwindow * window {nullptr};

};

