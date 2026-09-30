#pragma once

#include <cstdint>

namespace wcl::types {
    
    enum class ECollisionLayers : std::uint8_t {
        layer0=0b00000001,
        layer1=0b00000010,
        layer2=0b00000100,
        layer3=0b00001000,
        layer4=0b00010000,
        layer5=0b00100000,
        layer6=0b01000000,
        layer7=0b10000000,
    };

    inline constexpr ECollisionLayers operator|(ECollisionLayers a, ECollisionLayers b) {
        return static_cast<ECollisionLayers>(
            static_cast<std::uint8_t>(a) | static_cast<std::uint8_t>(b)
            );
    }

    inline constexpr ECollisionLayers & operator|=(ECollisionLayers & a, ECollisionLayers b) {
        a = static_cast<ECollisionLayers>(
            static_cast<std::uint8_t>(a) | static_cast<std::uint8_t>(b)
            );
        
        return a;
    }

    inline constexpr ECollisionLayers operator&(ECollisionLayers a, ECollisionLayers b) {
        return static_cast<ECollisionLayers>(
            static_cast<std::uint8_t>(a) & static_cast<std::uint8_t>(b)
            );
    }

    inline constexpr ECollisionLayers & operator&=(ECollisionLayers & a, ECollisionLayers b) {
        a = static_cast<ECollisionLayers>(
            static_cast<std::uint8_t>(a) & static_cast<std::uint8_t>(b)
            );
        return a;
    }

    inline constexpr ECollisionLayers operator^(ECollisionLayers a, ECollisionLayers b) {
        return static_cast<ECollisionLayers>(
            static_cast<std::uint8_t>(a) ^ static_cast<std::uint8_t>(b)
            );
    }

    inline constexpr ECollisionLayers & operator^=(ECollisionLayers & a, ECollisionLayers b) {
        a = static_cast<ECollisionLayers>(
            static_cast<std::uint8_t>(a) ^ static_cast<std::uint8_t>(b)
            );
        return a;
    }

    inline constexpr ECollisionLayers operator~(ECollisionLayers a) {
        return static_cast<ECollisionLayers>(
            ~static_cast<std::uint8_t>(a)
            );
    }

}
