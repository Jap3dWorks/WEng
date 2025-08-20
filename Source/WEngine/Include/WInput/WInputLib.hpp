#pragma once

#include "WCore/WCore.hpp"
#include "WStructs/WEngineStructs.hpp"

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#include <GLFW/glfw3.h>

namespace WInputLib {

    constexpr EInputKey ToInputKey(std::int32_t key) {
        switch(key) {
        case GLFW_KEY_A:
            return EInputKey::Key_A;
        case GLFW_KEY_B:
            return EInputKey::Key_B;
        case GLFW_KEY_C:
            return EInputKey::Key_C;
        case GLFW_KEY_D:
            return EInputKey::Key_D;
        case GLFW_KEY_E:
            return EInputKey::Key_E;
        case GLFW_KEY_F:
            return EInputKey::Key_F;
        case GLFW_KEY_G:
            return EInputKey::Key_G;
        case GLFW_KEY_H:
            return EInputKey::Key_H;
        case GLFW_KEY_I:
            return EInputKey::Key_I;
        case GLFW_KEY_J:
            return EInputKey::Key_J;
        case GLFW_KEY_K:
            return EInputKey::Key_K;
        case GLFW_KEY_L:
            return EInputKey::Key_L;
        case GLFW_KEY_M:
            return EInputKey::Key_M;
        case GLFW_KEY_N:
            return EInputKey::Key_N;
        case GLFW_KEY_O:
            return EInputKey::Key_O;
        case GLFW_KEY_P:
            return EInputKey::Key_P;
        case GLFW_KEY_Q:
            return EInputKey::Key_Q;
        case GLFW_KEY_R:
            return EInputKey::Key_R;
        case GLFW_KEY_S:
            return EInputKey::Key_S;
        case GLFW_KEY_T:
            return EInputKey::Key_T;
        case GLFW_KEY_U:
            return EInputKey::Key_U;
        case GLFW_KEY_V:
            return EInputKey::Key_V;
        case GLFW_KEY_W:
            return EInputKey::Key_W;
        case GLFW_KEY_X:
            return EInputKey::Key_X;
        case GLFW_KEY_Y:
            return EInputKey::Key_Y;
        case GLFW_KEY_Z:
            return EInputKey::Key_Z;
        case GLFW_KEY_0:
            return EInputKey::Key_0;
        case GLFW_KEY_1:
            return EInputKey::Key_1;
        case GLFW_KEY_2:
            return EInputKey::Key_2;
        case GLFW_KEY_3:
            return EInputKey::Key_3;
        case GLFW_KEY_4:
            return EInputKey::Key_4;
        case GLFW_KEY_5:
            return EInputKey::Key_5;
        case GLFW_KEY_6:
            return EInputKey::Key_6;
        case GLFW_KEY_7:
            return EInputKey::Key_7;
        case GLFW_KEY_8:
            return EInputKey::Key_8;
        case GLFW_KEY_9:
            return EInputKey::Key_9;
        default:
            return EInputKey::None;
        }
    }

    constexpr EInputMode ToInputMode(const std::int32_t & value) {
        switch(value) {
        case GLFW_PRESS:
            return EInputMode::Press;
        case GLFW_RELEASE:
            return EInputMode::Release;
        case GLFW_REPEAT:
            return EInputMode::Repeat;
        default:
            return EInputMode::None;
        }
    }

    constexpr WInput ToWInputMode(const std::int32_t & in_key, const std::int32_t & in_mode) {
        return {ToInputKey(in_key), ToInputMode(in_mode)};
    }

}
