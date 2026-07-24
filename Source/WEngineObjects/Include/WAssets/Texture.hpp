#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WAsset.hpp"
#include "WCoreTypes/WTexture.hpp"

#include <cassert>
#include <cstring>

#include "was::Texture.WEng.hpp"

using WTextureData = std::vector<uint8_t>;

namespace was {
    
    class WCORE_API Texture : public WAsset
    {
        WOBJECT_BODY;

    public:

        WPROPERTY(WTextureData, data_,);
        WPROPERTY(wct::texture::ETextureFormat, format,);
        WPROPERTY(std::uint32_t, width,);
        WPROPERTY(std::uint32_t, height,);
        WPROPERTY(wct::texture::ESampler, sampler,
                  wct::texture::ESampler::MIN_LINEAR   |
                  wct::texture::ESampler::MAG_LINEAR   |
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
            size_t channels = wct::texture::NumOfChannels(format);
            if (channels == 4) return;

            assert(channels == 3 && "Only RGB (3 channels) to RGBA conversion is supported");
            size_t pixel_count = static_cast<size_t>(width) * height;
            std::vector<uint8_t> rgba(pixel_count * 4, 255);

            const uint8_t* src = data_.data();
            uint8_t* dst = rgba.data();
            for (size_t i = 0; i < pixel_count; ++i) {
                dst[4*i + 0] = src[3*i + 0];
                dst[4*i + 1] = src[3*i + 1];
                dst[4*i + 2] = src[3*i + 2];
                dst[4*i + 3] = 255;
            }

            data_ = std::move(rgba);
            format = format | wct::texture::ETextureFormat::A;
        }
    };

}
