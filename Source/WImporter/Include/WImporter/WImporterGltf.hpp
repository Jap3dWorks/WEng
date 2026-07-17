#pragma once

#include "WImporter/WImporter.hpp"

namespace wim::importer {
    
    class WIMPORTER_API WImporterGltf : public WImporter {
    public:
        
        WImporterGltf() = delete;
        
        WImporterGltf(const WImporterGltf&) = default;
        WImporterGltf(WImporterGltf&&) noexcept = default;
        WImporterGltf& operator=(const WImporterGltf&) = default;
        WImporterGltf& operator=(WImporterGltf&&) noexcept = default;
        virtual ~WImporterGltf() = default;

        WImporterGltf(wcr::wid::WAssetId pbr_opaque,
                      wcr::wid::WAssetId pbr_param,
                      wcr::wid::WAssetId transparent_pipeline,
                      wcr::wid::WAssetId null_texture,
                      wcr::wid::WAssetId null_rgba,
                      wcr::wid::WAssetId null_normal) :
            render_pipelines_(
                {pbr_opaque,
                 pbr_param,
                 transparent_pipeline,
                 
                }),
            textures_(
                {null_texture,
                 null_rgba,
                 null_normal}
                )
        {}

    public:

        std::vector<wcr::wid::WAssetId> Import(
            WAssetDb & in_asset_manager,
            std::string_view file_path,
            std::string_view asset_directory
            ) override;

        std::vector<std::string_view> Extensions() const noexcept override;
        std::vector<std::string_view> Formats() const noexcept override;

        virtual std::unique_ptr<WImporter> Clone() override;

    private:

        struct RenderPipelines {
            wcr::wid::WAssetId pbr_opaque{};
            wcr::wid::WAssetId pbr_param{};
            wcr::wid::WAssetId transparent{};  // transparent pbr.
        } render_pipelines_{};

        struct TextureAssets {
            wcr::wid::WAssetId null_texture{};
            wcr::wid::WAssetId null_rgba{};
            wcr::wid::WAssetId null_normal{};
            
        } textures_{};

    };
    
}
