#pragma once

#include "WCore/WCore.hpp"
#include "WCoreTypes/WCoreTypes.hpp"
#include "WLog.hpp"

#include <string>
#include <cstdint>
#include <utility>
#include <vulkan/vulkan_core.h>

struct GLFWwindow;

namespace wdw {

    struct WWindowSize {
        std::uint32_t width{1280};
        std::uint32_t height{720};
    };

    class WWINDOW_API WWindow {
    public:

        using FramebufferSizeFun = void(*)(WWindow*, int width, int height);

        using KeyFun = void(*)(WWindow*, int key, int scancode, int action, int mods);

        using CursorPosFun = void(*)(WWindow*, double x, double y);

    private:

        struct Callbacks {
            std::vector<FramebufferSizeFun> framebuffer_size{};
            std::vector<KeyFun> key{};
            std::vector<CursorPosFun> cursor_pos{};
        };

    public:

        WWindow() {
            // Initialize();
        }

        WWindow(const WWindow&) = delete;
        WWindow& operator=(const WWindow&) = delete;

        WWindow(WWindow&& other) noexcept :
        title_(std::move(other.title_)),
        window_(other.window_) {

            static_windows_[window_]=this;
            static_callbacks_[this] = std::move(static_callbacks_[&other]);

            other.window_ = nullptr;
        }

        WWindow& operator=(WWindow&& other) noexcept {
            if (this != &other) {

                title_ = std::move(other.title_);
                window_ = other.window_;

                static_windows_[window_]=this;
                static_callbacks_[this] = std::move(static_callbacks_[&other]);

                other.window_ = nullptr;
            }

            return *this;
        }

        virtual ~WWindow() {
            Destroy();
        }

    public:

        void Initialize();

        WNODISCARD bool IsValid() const {
            return window_ != nullptr;
        }

        void SetFramebufferSizeCallback(
            FramebufferSizeFun callback
            );

        void SetKeyCallback(
            KeyFun callback
            );

        void SetCursorPosCallback(
            CursorPosFun callback
            );

        void SetWindowUserPtr(void* usr_ptr);

        void* GetWindowUserPtr() const;

        WWindowSize GetFramebufferSize() const;

        bool ShouldClose() const;

        void PollEvents();

        void CreateWindowSurface(
            VkSurfaceKHR & out_surface,
            VkInstance in_instance
            ) const; 

    private:

        static void SizeCallbackManager(GLFWwindow* glfw_window, int width, int height);

        static void CursorPosCallbackManager(GLFWwindow * glfw_window, double in_x, double in_y);

        static void KeyCallbackManager(GLFWwindow * glfw_window,
                                          int in_key,
                                          int in_scancode,
                                          int in_action,
                                          int in_mods);

        // void Initialize();
        
        void Destroy();

    private:

        static std::unordered_map<GLFWwindow*, WWindow*> static_windows_;
        static std::unordered_map<WWindow*, Callbacks> static_callbacks_;

        std::string title_ {"WEng"};

        // WWindowSize window_size_{};

        // GLFWframebuffersizefun framebuffer_size_callback {nullptr};
        // void * user_pointer_ {nullptr};

        GLFWwindow * window_ {nullptr};

    };
    
}
