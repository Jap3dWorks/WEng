#pragma once

#include "WCore/WCore.hpp"
#include "WEngineInterfaces/IImporter.hpp"
#include "WCore/TRef.hpp"
#include "WEngineObjects/TWRef.hpp"
#include "WEngineObjects/WAsset.hpp"

#include <vector>
#include <memory>

class WObjectManager;
class WAssetManagerFacade;

class WIMPORTERS_API WImporter : public IImporter {

public:

    explicit WImporter(WAssetManagerFacade & in_object_manager) noexcept;

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
    std::vector<WId> Import(const char* file_path, const char* asset_directory) override = 0;

    std::unique_ptr<IImporter> Clone() const override = 0;

    constexpr std::vector<std::string> Extensions() const noexcept override = 0;

    constexpr std::vector<std::string> Formats() const noexcept override = 0;

protected:

    WAssetManagerFacade & AssetManager();

private:

    TRef<WAssetManagerFacade> asset_manager_{nullptr};
    
};

class WIMPORTERS_API WImportObj : public WImporter
{
    
public:
    
    explicit WImportObj(WAssetManagerFacade & in_object_manager) noexcept;

    virtual ~WImportObj() = default;

    WImportObj(const WImportObj & in_other) noexcept = default;

    WImportObj(WImportObj && out_other) noexcept = default;
    
    WImportObj & operator=(const WImportObj & in_other) noexcept = default;

    WImportObj & operator=(WImportObj && out_other) noexcept = default;

public:

    std::vector<WId> Import(const char * file_path, const char * asset_directory) override;

    std::unique_ptr<IImporter> Clone() const override;

    constexpr std::vector<std::string> Extensions() const noexcept override {
        return {".obj"};
    }

    constexpr std::vector<std::string> Formats() const noexcept override {
        return {"obj"};
    }
};

class WIMPORTERS_API WImportTexture : public WImporter
{
    
public:

    explicit WImportTexture(WAssetManagerFacade & in_object_manager) noexcept;

    virtual ~WImportTexture() = default;

    WImportTexture(const WImportTexture & in_other) noexcept = default;

    WImportTexture(WImportTexture && out_other) noexcept = default;
    
    WImportTexture & operator=(const WImportTexture & in_other) noexcept = default;

    WImportTexture & operator=(WImportTexture && out_other) noexcept = default;

public:
    
    std::vector<WId> Import(const char * file_path, const char * asset_directory) override;

    std::unique_ptr<IImporter> Clone() const override;

    constexpr std::vector<std::string> Extensions() const noexcept override {
        return {".png", ".jpg", ".jpeg", ".tga", ".bmp"};
    }

    constexpr std::vector<std::string> Formats() const noexcept override {
        return {"png", "jpg", "jpeg", "tga", "bmp"};
    }
};

