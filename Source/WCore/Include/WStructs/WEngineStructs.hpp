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

enum class EInputs {
    A, B, C, D,
    E, F, G, H,
    I, J, K, L,
    M, N, O, P,
    Q, R, S, T,
    U, V, W, X,
    Y, Z,
    k0, k1, k2, k3,
    k4, k5, k6, k7,
    k8, k9,
    MouseMove,
    MouseK0,
    MouseK1,
    MouseK2,
    Ctrl,
    Shift,
    Alt,
    Space,
    Tab
};

struct WInputValuesStruct {
    EInputs input{};
    float presure{};
    glm::vec2 direction{};
};

struct WInputMapStruct {
    // EInput ActionsAssets
    std::unordered_map<EInputs, std::vector<WAssetId>> map{};  
};

struct WActionStruct {
    char name[32];
};

struct WActionDataStruct {
    WAssetId input_map_asset;
    WAssetId action_asset;
};

