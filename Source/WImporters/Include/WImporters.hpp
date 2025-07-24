#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TOptionalRef.hpp"

#include <vector>

class WObjectDb;
class WAssetDb;

class WIMPORTERS_API WImporter {

public:

    explicit WImporter() noexcept;

    virtual ~WImporter() = default;

    WImporter(const WImporter & in_other) noexcept;
    
    WImporter(WImporter && out_other) noexcept;

    WImporter & operator=(const WImporter & in_other) noexcept;

    WImporter & operator=(WImporter && out_other) noexcept;

public:    

    /**
     * @brief Import an asset from a file
     * @param file_path The file path to import
     * @param asset_directory, Engine based directory path to import to, Should start with /Content.
    */
    virtual std::vector<WId> Import(
        WAssetDb & in_asset_manager,
        const char* file_path,
        const char* asset_directory) = 0;

    virtual constexpr std::vector<std::string> Extensions() const noexcept = 0;

    virtual constexpr std::vector<std::string> Formats() const noexcept = 0;

    virtual std::unique_ptr<WImporter> Clone()=0;

    TOptionalRef<WAssetDb> AssetManager();

    void AssetManager(WAssetDb & asset_manager);

private:

    TOptionalRef<WAssetDb> asset_manager_{nullptr};
    
};

class WIMPORTERS_API WImportObj final : public WImporter
{
    
public:
    
    explicit WImportObj() noexcept;

    virtual ~WImportObj() = default;

    WImportObj(const WImportObj & in_other) noexcept = default;

    WImportObj(WImportObj && out_other) noexcept = default;
    
    WImportObj & operator=(const WImportObj & in_other) noexcept = default;

    WImportObj & operator=(WImportObj && out_other) noexcept = default;

    std::unique_ptr<WImporter> Clone() override { return std::make_unique<WImportObj>(*this); }

public:

    std::vector<WId> Import(
        WAssetDb & in_asset_manager,
        const char * file_path,
        const char * asset_directory) override;

    constexpr std::vector<std::string> Extensions() const noexcept override {
        return {".obj"};
    }

    constexpr std::vector<std::string> Formats() const noexcept override {
        return {"obj"};
    }
};

class WIMPORTERS_API WImportTexture final : public WImporter
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
    
    std::vector<WId> Import(
        WAssetDb & in_asset_manager,
        const char * file_path,
        const char * asset_directory) override;

    constexpr std::vector<std::string> Extensions() const noexcept override {
        return {".png", ".jpg", ".jpeg", ".tga", ".bmp"};
    }

    constexpr std::vector<std::string> Formats() const noexcept override {
        return {"png", "jpg", "jpeg", "tga", "bmp"};
    }
};

