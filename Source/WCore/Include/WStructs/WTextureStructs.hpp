#pragma once

#include <cstdint>
#include <vector>


enum class ETextureChannels : uint8_t
{
    // 8 bit
    // R=1,G=2,B=4,A=8
    kR=1,
    kRG=3,
    kRGB=7,
    kRGBA=15,
    // 16 bit
    // d_16=16,
    kR16=17,
    kRG16=19,
    kRGB16=23,
    kRGBA16=31,
    // 32 bit
    // d_32=32,
    kR32=33,
    kRG32=35,
    kRGB32=39,
    kRGBA32=47,
};

inline ETextureChannels operator|(ETextureChannels a, ETextureChannels b)
{
    return static_cast<ETextureChannels>(
        static_cast<uint8_t>(a) | static_cast<uint8_t>(b)
    );
}

inline ETextureChannels operator&(ETextureChannels a, ETextureChannels b)
{
    return static_cast<ETextureChannels>(
        static_cast<uint8_t>(a) & static_cast<uint8_t>(b)
    );
}

inline uint8_t NumOfChannels(ETextureChannels channels)
{
    switch(channels) {
    case ETextureChannels::kR:
        return 1;
    case ETextureChannels::kRG:
        return 2;
    case ETextureChannels::kRGB:
        return 3;
    case ETextureChannels::kRGBA:
        return 4;
    default:
        return 0;
    }
}

struct WTextureStruct{
    std::vector<uint8_t> data;
    ETextureChannels channels;
    uint32_t width;
    uint32_t height;    

    WTextureStruct()=default;
    ~WTextureStruct()=default;
    WTextureStruct(const WTextureStruct & other) {
        data = other.data;
        channels = other.channels;
        width = other.width;
        height = other.height;
    }
    WTextureStruct(WTextureStruct && other) {
        data = std::move(other.data);
        channels = std::move(other.channels);
        width = std::move(other.width);
        height = std::move(other.height);
    }
    WTextureStruct& operator=(const WTextureStruct & other) {
        data = other.data;
        channels = other.channels;
        width = other.width;
        height = other.height;
        return *this;
    }
    WTextureStruct& operator=(WTextureStruct && other) {
        data = std::move(other.data);
        channels = std::move(other.channels);
        width = std::move(other.width);
        height = std::move(other.height);
        return *this;
    }
};
