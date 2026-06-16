#pragma once

#include <cstdint>
#include <vector>
#include <cstring>

namespace wct::texture {

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

    struct WTexture {
        std::vector<uint8_t> data;
        ETextureFormat format;
        uint32_t width;
        uint32_t height;
    };

    inline WTexture AddRGBAPadding(const WTexture & in_texture) {
        wct::texture::WTexture result;

        wct::texture::ETextureFormat all_channels_format =
            in_texture.format | 
            wct::texture::ETextureFormat::R |
            wct::texture::ETextureFormat::G |
            wct::texture::ETextureFormat::B |
            wct::texture::ETextureFormat::A;

        result.format = all_channels_format;
        result.height = in_texture.height;
        result.width = in_texture.width;

        result.data = in_texture.data;

        size_t tex_size = result.height * result.width;

        result.data.resize(tex_size * 4, 255);

        int num_channels = wct::texture::NumOfChannels(in_texture.format);

        for (int i=num_channels; i < 4; i++) {
            std::memcpy(
                result.data.data() + (tex_size * i),
                result.data.data(),
                tex_size
                );
        }

        return result;
    }

}
