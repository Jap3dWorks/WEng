#include "WImporter/WImporterTexture.hpp"
#include "WCoreTypes/WTexture.hpp"
#include "WAssets/WTextureAsset.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WString/WString.hpp"
#include "WLib_stbi.hpp"

// WImportTexture
// --------------

wim::importer::WImportTexture::WImportTexture() noexcept {}

std::vector<wid::WAssetId> wim::importer::WImportTexture::Import(
    WAssetDb & in_asset_manager,
    std::string_view file_path,
    std::string_view asset_directory)
{
    auto stbi_image = wim::WLib_wtbi::LoadPath(file_path);
        
    if (!stbi_image.pixels)
    {
        throw std::runtime_error("Failed to load texture image!");
    }

    wid::WAssetId id = in_asset_manager.Create<WTextureAsset>(
        wstr::AssetPath(
            std::string(asset_directory),
            std::string(file_path),
            "texture").c_str()
        );

    auto & asset = in_asset_manager.Get<WTextureAsset>(id);

    asset.SetTextureData(
        stbi_image.pixels.get(),
        stbi_image.width,
        stbi_image.height,
        stbi_image.format
        );

    return { id };
}


