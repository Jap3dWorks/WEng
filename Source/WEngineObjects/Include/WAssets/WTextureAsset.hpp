#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WAsset.hpp"
#include "WCoreTypes/WTexture.hpp"
#include <memory>

#include "WTextureAsset.WEngine.hpp"

using WTextureData = std::vector<uint8_t>;

class WCORE_API WTextureAsset : public WAsset
{
    WOBJECT_BODY;

public:

    // WPROPERTY(wct::texture::WTexture, texture, );

    WPROPERTY(WTextureData, data_,);
    WPROPERTY(wct::texture::ETextureFormat, format,);
    WPROPERTY(std::uint32_t, width,);
    WPROPERTY(std::uint32_t, height,);

public:

    void SetTextureData(
        std::uint8_t * in_ptr,
        std::uint32_t in_width,
        std::uint32_t in_height,
        wct::texture::ETextureFormat in_format
        ) {
        format = in_format;
        std::size_t size =
            in_width *
            in_height *
            wct::texture::NumOfChannels(in_format) 
            // TODO * color depth bytes
            ;
        
        data_.resize(size);

        std::memcpy(
            data_.data(),
            in_ptr,
            size
            );
    }

    std::uint8_t const * GetTexturePtr() const {
        return data_.data();
    }

    std::size_t GetTextureDataSize() const {
        return data_.size();
    }

    // void SetTexture(const wct::texture::WTexture& texture);

    // void SetTexture(wct::texture::WTexture&& texture);

    // const wct::texture::WTexture& GetTexture() const;

    inline void AddRGBAPadding() {
        // wct::texture::WTexture result;

        wct::texture::ETextureFormat all_channels_format =
            format | 
            wct::texture::ETextureFormat::R |
            wct::texture::ETextureFormat::G |
            wct::texture::ETextureFormat::B |
            wct::texture::ETextureFormat::A;

        // result.data = in_texture.data;

        size_t tex_size = height * width; // TODO * depth

        // TODO * color depth
        data_.resize(tex_size * 4, 255);

        int num_channels = wct::texture::NumOfChannels(format);

        for (int i=num_channels; i < 4; i++) {
            std::memcpy(
                data_.data() + (tex_size * i),
                data_.data(),
                tex_size
                );
        }

        format = all_channels_format;
    }    

private:

};

