#pragma once

#include <cstdint>
#include <vector>

namespace wtp::texture {

   /**
    * 8 bits, significative bit to left
    *   8765 - 4321
    *
    * [16bit] = 128(bit 8) (high range)
    * IF bit 8 active THEN
    *    [32bit]=64 (bit 7)
    * ELSE
    *    [SNORM]= 64 (bit 6)
    *    [sRGB] = 32 (bit 7)
    *
    * [R channel]=8 (bit 4), [G channel]=4 (bit 3),
    * [B channel]=2 (bit 2), [A channel]=1 (bit 1)
    */
    enum class ETextureFormat : uint8_t {

        // 8 bit UNORM, good for linear textures like roughness, clamp range is [0,1]
        R8_UNORM=8,
        RG8_UNORM=8+4,
        RGB8_UNORM=8+4+2,
        RGBA8_UNORM=8+4+2+1,

        // 8 bit SNORM, good for normal maps (color*2 - 1 tansform), clamp range is [-1,1]
        R8_SNORM=64 + 8,
        RG8_SNORM=64 + 8 + 4,
        RGB8_SNORM=64 + 8 + 4 + 2,
        RGBA8_SNORM=64 + 8 + 4 + 2 + 1,
    
        // 8 bit sRGB, good for albedo textures 
        R8_SRGB=32 + 8,
        RG8_SRGB=32 + 8 + 4,
        RGB8_SRGB=32 + 8 + 4 + 2,
        RGBA8_SRGB=32 + 8 + 4 + 2 + 1,

        // 16 bit
        R16_SFLOAT=128 + 8,
        RG16_SFLOAT=128 + 8 + 4,
        RGB16_SFLOAT=128 + 8 + 4 + 2,
        RGBA16_SFLOAT=128 + 8 + 4 + 2 + 1,

        // 32 bit
        R32_SFLOAT=128 + 64 + 8,
        RG32_SFLOAT=128 + 64 + 8 + 4,
        RGB32_SFLOAT=128 + 64 + 8 + 4 + 2,
        RGBA32_SFLOAT=128 + 64 + 8 + 4 + 2 + 1,
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

    struct WTexture {
        std::vector<uint8_t> data;
        ETextureFormat format;
        uint32_t width;
        uint32_t height;
    };

}
