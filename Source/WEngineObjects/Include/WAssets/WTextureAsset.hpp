#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WAsset.hpp"
#include "WCoreTypes/WTexture.hpp"
#include <algorithm>
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

        data_.assign(in_ptr, in_ptr + size);

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

        std::uint8_t color_depth_bytes = wct::texture::ColorDepth(format) / 8;

        size_t pixel_count = static_cast<size_t>(width) * height;
        size_t bytes_per_channel = pixel_count;
        size_t old_planes = wct::texture::NumOfChannels(format);

        if (old_planes == 4) return;

        // Allocate new vector for 4 planes
        std::vector<uint8_t> new_data(pixel_count * 4 * color_depth_bytes);

        const uint8_t* src = data_.data();
        uint8_t* dst = new_data.data();
        std::memcpy(dst, src, bytes_per_channel * old_planes * color_depth_bytes);

        // Fill alpha plane with 255
        std::fill(dst + bytes_per_channel * old_planes * color_depth_bytes,
                  dst + bytes_per_channel * 4 * color_depth_bytes,
                  255);

        // Replace and update format
        data_ = std::move(new_data);
        format = all_channels_format;
    }    
};

