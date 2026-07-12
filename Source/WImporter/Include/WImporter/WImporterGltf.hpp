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

        WImporterGltf(wid::WAssetId pbr_pipeline, wid::WAssetId transparent_pipeline) :
            render_pipelines_({pbr_pipeline, transparent_pipeline}) {}

    public:

        std::vector<wid::WAssetId> Import(
            WAssetDb & in_asset_manager,
            std::string_view file_path,
            std::string_view asset_directory
            ) override;

        std::vector<std::string_view> Extensions() const noexcept override;
        std::vector<std::string_view> Formats() const noexcept override;

        virtual std::unique_ptr<WImporter> Clone() override;

    private:

        struct RenderPipelines {
            wid::WAssetId gbuffer{};      // opaque pbr.
            wid::WAssetId transparent{};  // transparent pbr.
        } render_pipelines_{};

        

    };
    
}
