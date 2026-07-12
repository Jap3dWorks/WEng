#pragma once

#include "WImporter/WImporter.hpp"

namespace wim::importer {

    class WIMPORTER_API WImportTexture final : public WImporter
    {
    
    public:

        explicit WImportTexture() noexcept;

        virtual ~WImportTexture() = default;

        WImportTexture(const WImportTexture & in_other) noexcept = default;

        WImportTexture(WImportTexture && out_other) noexcept = default;
    
        WImportTexture & operator=(const WImportTexture & in_other) noexcept = default;

        WImportTexture & operator=(WImportTexture && out_other) noexcept = default;

        std::unique_ptr<WImporter> Clone() override { return std::make_unique<WImportTexture>(*this); }

    public:
    
        std::vector<wid::WAssetId> Import(
            WAssetDb & in_asset_manager,
            std::string_view file_path,
            std::string_view asset_directory) override;

        constexpr std::vector<std::string_view> Extensions() const noexcept override {
            return {".png", ".jpg", ".jpeg", ".tga", ".bmp"};
        }

        constexpr std::vector<std::string_view> Formats() const noexcept override {
            return {"png", "jpg", "jpeg", "tga", "bmp"};
        }
    };

}
