#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WAsset.hpp"
#include "WCoreTypes/WTexture.hpp"
#include <cstring>

#include "WTextureAsset.WEngine.hpp"

using WTextureData = std::vector<uint8_t>;

class WCORE_API WTextureAsset : public WAsset
{
    WOBJECT_BODY;

public:

    WPROPERTY(WTextureData, data_,);
    WPROPERTY(wct::texture::ETextureFormat, format,);
    WPROPERTY(std::uint32_t, width,);
    WPROPERTY(std::uint32_t, height,);
    WPROPERTY(wct::texture::ESampler, sampler,
              wct::texture::ESampler::MIN_BILINEAR |
              wct::texture::ESampler::MAG_BILINEAR |
              wct::texture::ESampler::WRAPS_REPEAT |
              wct::texture::ESampler::WRAPT_REPEAT
        );

public:

    void SetTextureData(
        std::uint8_t * in_ptr,
        std::uint32_t in_width,
        std::uint32_t in_height,
        wct::texture::ETextureFormat in_format
        ) {
        format = in_format;
        width = in_width;
        height = in_height;
        
        std::uint8_t color_depth_bytes = wct::texture::ColorDepth(format) / 8;
        
        std::size_t size =
            in_width *
            in_height *
            wct::texture::NumOfChannels(in_format) *
            color_depth_bytes;
        
        data_.resize(size);

        std::memcpy(
            data_.data(),
            in_ptr,
            size
            );
    }

    std::uint8_t const * GetDataPtr() const {
        return data_.data();
    }

    std::size_t GetDataSize() const {
        return data_.size();
    }

    inline void AddRGBAPadding() {

        wct::texture::ETextureFormat all_channels_format =
            format | 
            wct::texture::ETextureFormat::R |
            wct::texture::ETextureFormat::G |
            wct::texture::ETextureFormat::B |
            wct::texture::ETextureFormat::A;

        size_t tex_size = height * width; // TODO * depth

        std::uint8_t color_depth_bytes = wct::texture::ColorDepth(format) / 8;

        data_.resize(tex_size * 4 * color_depth_bytes, 255);

        int num_channels = wct::texture::NumOfChannels(format);

        for (int i=num_channels; i < 4; i++) {
            std::memcpy(
                data_.data() + (tex_size * i * color_depth_bytes),
                data_.data(),
                tex_size * color_depth_bytes
                );
        }

        format = all_channels_format;
    }    
};

