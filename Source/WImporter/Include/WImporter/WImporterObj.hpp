#pragma once

#include "WImporter/WImporter.hpp"

namespace wim::importer {

    class WIMPORTER_API WImporterObj final : public WImporter
    {
    
    public:
    
        explicit WImporterObj() noexcept;

        virtual ~WImporterObj() = default;

        WImporterObj(const WImporterObj & in_other) noexcept = default;

        WImporterObj(WImporterObj && out_other) noexcept = default;
    
        WImporterObj & operator=(const WImporterObj & in_other) noexcept = default;

        WImporterObj & operator=(WImporterObj && out_other) noexcept = default;

        std::unique_ptr<WImporter> Clone() override {
            return std::make_unique<WImporterObj>(*this);
        }

    public:

        std::vector<WAssetId> Import(
            WAssetDb & in_asset_manager,
            std::string_view file_path,
            std::string_view asset_directory) override;

        constexpr std::vector<std::string_view> Extensions() const noexcept override {
            return {".obj"};
        }
        constexpr std::vector<std::string_view> Formats() const noexcept override {
            return {"obj"};
        }
    };


}
