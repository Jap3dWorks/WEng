#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TOptionalRef.hpp"

#include <vector>
#include <string_view>
#include <memory>

class WAssetDb;

namespace wim::importer {

    class WIMPORTER_API WImporter {

    public:

        explicit WImporter() noexcept=default;

        virtual ~WImporter() = default;

        WImporter(const WImporter & in_other) noexcept=default;

        WImporter(WImporter && out_other) noexcept=default;

        WImporter & operator=(const WImporter & in_other) noexcept=default;

        WImporter & operator=(WImporter && out_other) noexcept=default;

    public:    

        /**
         * @brief Import an asset from a file
         * @param file_path The file path to import
         * @param asset_directory, Engine based directory path to import to,
         * Should start with /Content.
         */
        virtual std::vector<wcr::wid::WAssetId> Import(
            WAssetDb & in_asset_manager,
            std::string_view file_path,
            std::string_view asset_directory) = 0;

        virtual constexpr std::vector<std::string_view> Extensions() const noexcept = 0;
        virtual constexpr std::vector<std::string_view> Formats() const noexcept = 0;

        virtual std::unique_ptr<WImporter> Clone()=0;

        TOptionalRef<WAssetDb> AssetManager();
    
    };

}
