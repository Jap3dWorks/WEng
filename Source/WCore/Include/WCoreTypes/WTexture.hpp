#pragma once

#include <cstdint>
#include <vector>
#include <cstring>

namespace wct::texture {

   /**
    * 8 bits, significative bit to left
    *   7654 - 3210
    *
    * [16bit] = 128(bit 7) (high range)
    * IF bit 7 active THEN
    *    [32bit]=64 (bit 6)
    * ELSE
    *    [SNORM]= 64 (bit 6)
    *    [sRGB] = 32 (bit 5)
    *
    * [R channel]=8 (bit 3), [G channel]=4 (bit 2),
    * [B channel]=2 (bit 1), [A channel]=1 (bit 0)
    */
    enum class ETextureFormat : uint8_t {

        R=8,
        G=4,
        B=2,
        A=1,

        // 8 bit UNORM, good for linear textures like roughness, clamp range is [0,1]
        R8_UNORM=R,
        RG8_UNORM=R+G,
        RGB8_UNORM=R+G+B,
        RGBA8_UNORM=R+G+B+A,

        // 8 bit SNORM, good for normal maps (color*2 - 1 tansform), clamp range is [-1,1]
        R8_SNORM=64 + R,
        RG8_SNORM=64 + R + G,
        RGB8_SNORM=64 + R + G + B,
        RGBA8_SNORM=64 + R + G + B + A,
    
        // 8 bit sRGB, good for albedo textures 
        R8_SRGB=32 + R,
        RG8_SRGB=32 + R + G,
        RGB8_SRGB=32 + R + G + B,
        RGBA8_SRGB=32 + R + G + B + A,

        // 16 bit
        R16_SFLOAT=128 + R,
        RG16_SFLOAT=128 + R + G,
        RGB16_SFLOAT=128 + R + G + B,
        RGBA16_SFLOAT=128 + R + G + B + A,

        // 32 bit
        R32_SFLOAT=128 + 64 + R,
        RG32_SFLOAT=128 + 64 + R + G,
        RGB32_SFLOAT=128 + 64 + R + G + B,
        RGBA32_SFLOAT=128 + 64 + R + G + B + A,
    };

    inline ETextureFormat operator|(ETextureFormat a, ETextureFormat b)
    {
        return static_cast<ETextureFormat>(
            static_cast<uint8_t>(a) | static_cast<uint8_t>(b)
            );
    }

    inline ETextureFormat operator&(ETextureFormat a, ETextureFormat b)
    {
        return static_cast<ETextureFormat>(
            static_cast<uint8_t>(a) & static_cast<uint8_t>(b)
            );
    }

    inline uint8_t NumOfChannels(ETextureFormat in_format)
    {
        switch(static_cast<uint8_t>(in_format) & (8 + 4 + 2 + 1)) {
        case 8:
            return 1;
        case 8 + 4:
            return 2;
        case 8 + 4 + 2:
            return 3;
        case 8 + 4 + 2 + 1:
            return 4;
        default:
            return 0;
        }
    }

    inline std::uint8_t ColorDepth(ETextureFormat in_format) {
        switch(static_cast<uint8_t>(in_format) & (128 + 64)) {
        case 128 + 64:
            return 32;
        case 128:
            return 16;
        default:
            return 8;
        }
    }
}
