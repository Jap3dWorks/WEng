#pragma once

#include "WCore/WCore.hpp"

#include <string>
#include <glm/glm.hpp>

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#include <GLFW/glfw3.h>

class WLevel;

struct WEngineCycleStruct {
    float DeltaTime{999};
    float TotalTime{0};
    WLevel * level{nullptr};  
};

struct WWindowStruct
{

    std::string title {"WEngine"};
    uint32_t width {800};
    uint32_t height {600};

    // GLFWframebuffersizefun framebuffer_size_callback {nullptr};
    void * user_pointer {nullptr};

    GLFWwindow * window {nullptr};

};

enum class EInputKey : std::uint8_t{
    None,
    Key_A, Key_B, Key_C, Key_D,
    Key_E, Key_F, Key_G, Key_H,
    Key_I, Key_J, Key_K, Key_L,
    Key_M, Key_N, Key_O, Key_P,
    Key_Q, Key_R, Key_S, Key_T,
    Key_U, Key_V, Key_W, Key_X,
    Key_Y, Key_Z,
    Key_0, Key_1, Key_2, Key_3,
    Key_4, Key_5, Key_6, Key_7,
    Key_8, Key_9,
    Mouse_Move,
    Mouse_Scroll,
    Mouse_Key_0,
    Mouse_Key_1,
    Mouse_Key_2,
    Key_Ctrl,
    Key_Shift,
    Key_Alt,
    Key_Space,
    Key_Tab
};

enum class EInputMode : std::uint8_t {
    None,
    Press,
    Release,
    Repeat
};

struct WInputMode {
    EInputKey input;
    EInputMode mode;
};

namespace std {
    template<>
    struct hash<WInputMode>
    {
        std::size_t operator()(const WInputMode & input) const {
            std::size_t r=0;

            r |= static_cast<std::uint8_t>(input.input);
            r <<= 8;
            r |= static_cast<std::uint8_t>(input.mode);

            return r;
        }
    };
}

struct WInputValuesStruct {
    WInputMode input{};
    float presure{};
    glm::vec2 direction{};
};

struct WInputMapStruct {
    // EInput ActionsAssets
    std::unordered_map<WInputMode, std::vector<WAssetId>> map{};  
};

struct WActionStruct {
    char name[32];
};

struct WActionDataStruct {
    WAssetId input_map_asset;
    WAssetId action_asset;
};

