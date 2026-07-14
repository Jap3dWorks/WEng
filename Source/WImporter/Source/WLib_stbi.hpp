#pragma once

#include "WCore/WCore.hpp"
#include "WCoreTypes/WTexture.hpp"

#include <string_view>
#include <string>
#include <cstdint>
#include <memory>

#include <stb/stb_image.h>

namespace wim::WLib_wtbi {

    using Pixels=std::unique_ptr<stbi_uc,decltype(&stbi_image_free)>;

    struct WStbiImage {
        Pixels pixels{nullptr, &stbi_image_free};
        std::uint32_t width{0};
        std::uint32_t height{0};
        wct::texture::ETextureFormat format{};
    };

    WNODISCARD inline constexpr wct::texture::ETextureFormat GetFormat(std::uint32_t num_channels) {
        // Default formats
        switch(num_channels) {
        case 1:
            return wct::texture::ETextureFormat::R8_UNORM;
        case 2:
            return wct::texture::ETextureFormat::RG8_UNORM;
        case 3:
            return wct::texture::ETextureFormat::RGB8_SRGB;
        case 4:
            return wct::texture::ETextureFormat::RGBA8_SRGB;
        default:
            return wct::texture::ETextureFormat::RGBA8_SRGB;
        }            
    }

    WNODISCARD inline WStbiImage LoadPath (std::string_view in_path) {
        int width, height, num_channels;

        stbi_uc * pixels = stbi_load(
            std::string(in_path).c_str(),
            &width,
            &height,
            &num_channels,
            0
            );

        return {
            .pixels={pixels, &stbi_image_free},
            .width=static_cast<std::uint32_t>(width),
            .height=static_cast<std::uint32_t>(height),
            .format=GetFormat(num_channels)
        };
    }

    WNODISCARD inline WStbiImage LoadBuffer(const std::byte * in_buffer, int in_size) {
        int width, height, num_channels;

        stbi_uc * pixels = stbi_load_from_memory(
            reinterpret_cast<stbi_uc const *>(in_buffer),
            in_size,
            &width,
            &height,
            &num_channels,
            0
            );

        return {
            .pixels={pixels, stbi_image_free},
            .width=static_cast<std::uint32_t>(width),
            .height=static_cast<std::uint32_t>(height),
            .format=GetFormat(num_channels)
        };
    }


}
