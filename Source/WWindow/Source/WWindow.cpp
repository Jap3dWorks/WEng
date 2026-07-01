#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <stdexcept>
#include <vk_video/vulkan_video_codec_h265std.h>
#include <vulkan/vulkan_core.h>
#endif
#include <GLFW/glfw3.h>

#include "WWindow/WWindow.hpp"

std::unordered_map<GLFWwindow*, wdw::WWindow*> wdw::WWindow::static_windows_{};
std::unordered_map<wdw::WWindow*, wdw::WWindow::Callbacks> wdw::WWindow::static_callbacks_{};

void wdw::WWindow::SizeCallbackManager(GLFWwindow* glfw_window, int width, int height) {
    WWindow* wwindow = static_windows_[glfw_window];

    for (auto ptr : static_callbacks_[wwindow].framebuffer_size) {
        ptr(wwindow, width, height);
    }
}

void wdw::WWindow::CursorPosCallbackManager(GLFWwindow * glfw_window, double in_x, double in_y) {
    WWindow* wwindow = static_windows_[glfw_window];

    for (auto ptr : static_callbacks_[wwindow].cursor_pos) {
        ptr(wwindow, in_x, in_y);
    }
}

void wdw::WWindow::KeyCallbackManager(GLFWwindow * glfw_window,
                                      int in_key,
                                      int in_scancode,
                                      int in_action,
                                      int in_mods) {
    WWindow * wwindow = static_windows_[glfw_window];

    for (auto ptr : static_callbacks_[wwindow].key) {
        ptr(wwindow, in_key, in_scancode, in_action, in_mods);
    }
}


void wdw::WWindow::SetFramebufferSizeCallback(
    wdw::WWindow::FramebufferSizeFun callback
    ) {
    static_callbacks_[this].framebuffer_size.push_back(callback);
}

void wdw::WWindow::SetKeyCallback(
    wdw::WWindow::KeyFun callback
    ) {
    static_callbacks_[this].key.push_back(callback);
}

void wdw::WWindow::SetCursorPosCallback(
    wdw::WWindow::CursorPosFun callback
    ) {
    static_callbacks_[this].cursor_pos.push_back(callback);
}


wdw::WWindowSize wdw::WWindow::GetFramebufferSize() const {

    int width, height;
    glfwGetFramebufferSize(window_, &width, &height);

    return {.width = static_cast<std::uint32_t>(width),
        .height = static_cast<std::uint32_t>(height)};
}

void wdw::WWindow::SetWindowUserPtr(void * in_ptr) {
    glfwSetWindowUserPointer(window_, in_ptr);
}

void* wdw::WWindow::GetWindowUserPtr() const {
    return glfwGetWindowUserPointer(window_);
}

bool wdw::WWindow::ShouldClose() const {
    return glfwWindowShouldClose(window_);
}

void wdw::WWindow::PollEvents() {
    glfwPollEvents();
}

void wdw::WWindow::CreateWindowSurface(VkSurfaceKHR & out_surface,
                                        VkInstance in_instance ) const {

    if(glfwCreateWindowSurface(
           in_instance,
           window_,
           nullptr,
           &out_surface
           ) != VK_SUCCESS) {
        throw std::runtime_error("Failed while creating Vulkan Surface KHR!");
    }
}

void wdw::WWindow::Initialize() {
    glfwSetErrorCallback(
        [](int code, const char* desc)
            {
                WLOG("GLFW Error {} : {}.", code, desc);
            });    

    // Force wayland session
    // glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);    

    if(!glfwInit()) {
        WFLOG("glfwInit failed.");
        return;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window_ = glfwCreateWindow(
        800,
        600,
        title_.c_str(),
        nullptr,
        nullptr
        );

    if (!window_) {
        WFLOG("glfwCreateWindow failed.");
        return;
    }

    // glfwSetWindowUserPointer(window_, this);

    static_windows_[window_] = this;
    static_callbacks_[this]={};

    glfwSetFramebufferSizeCallback(
        window_,
        &SizeCallbackManager
        );

    glfwSetKeyCallback(
        window_,
        &KeyCallbackManager
        );

    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window_, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(window_, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    glfwSetCursorPosCallback(window_, CursorPosCallbackManager);

    return;
}

void wdw::WWindow::Destroy() {
    if(window_ != nullptr) {

        static_windows_.extract(window_);
        static_callbacks_.erase(this);

        glfwDestroyWindow(window_);
        glfwTerminate();

        window_ = nullptr;
    }
}
