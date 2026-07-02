#include "WImporter/WImporterTexture.hpp"
#include "WCoreTypes/WTexture.hpp"
#include "WAssets/WTextureAsset.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WUtils/WStringUtils.hpp"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#endif

// WImportTexture
// --------------

wim::importer::WImportTexture::WImportTexture() noexcept {}

std::vector<WAssetId> wim::importer::WImportTexture::Import(
    WAssetDb & in_asset_manager,
    std::string_view file_path,
    std::string_view asset_directory)
{
    int width, height, num_channels;
    stbi_uc * Pixels = stbi_load(
        std::string(file_path).data(),
        &width,
        &height,
        &num_channels,
        STBI_rgb_alpha
    );

    if (!Pixels)
    {
        throw std::runtime_error("Failed to load texture image!");
    }

    // wct::texture::WTexture texture_struct = {};
    // texture_struct.width = width;
    // texture_struct.height = height;

    wct::texture::ETextureFormat format;

    // Default formats
    switch(num_channels) {
    case 1:
        format = wct::texture::ETextureFormat::R8_UNORM;
        break;
    case 2:
        format = wct::texture::ETextureFormat::RG8_UNORM;
        break;
    case 3:
        format = wct::texture::ETextureFormat::RGB8_SRGB;
        break;
    case 4:
        format = wct::texture::ETextureFormat::RGBA8_SRGB;
        break;
    default:
        format = wct::texture::ETextureFormat::RGBA8_SRGB;
    }

    WAssetId id = in_asset_manager.Create<WTextureAsset>(
        WStringUtils::AssetPath(
            std::string(asset_directory),
            std::string(file_path),
            "texture").c_str()
        );

    auto * asset = in_asset_manager.Get<WTextureAsset>(id);

    asset->SetTextureData(
        Pixels,
        width, height,
        format
        );

    stbi_image_free(Pixels);

    return { id };
}


