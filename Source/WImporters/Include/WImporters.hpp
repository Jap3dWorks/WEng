#pragma once

#include "WCore/WCore.hpp"
#include "IImporter.hpp"
#include "WCore/TRef.hpp"
#include "WEngineObjects/TWRef.hpp"
#include "WEngineObjects/WAsset.hpp"

#include <vector>
#include <memory>

class WObjectManager;
class WAssetManagerFacade;

class WIMPORTERS_API WImporter : public IImporter
{

public:

    constexpr explicit WImporter(WAssetManagerFacade & in_object_manager) noexcept;

    virtual ~WImporter() = default;

    constexpr WImporter(const WImporter & in_other) noexcept;
    
    constexpr WImporter(WImporter && out_other) noexcept;

    constexpr WImporter & operator=(const WImporter & in_other) noexcept;

    constexpr WImporter & operator=(WImporter && out_other) noexcept;

public:    

    /**
     * @brief Import an asset from a file
     * @param file_path The file path to import
     * @param asset_directory, Engine based directory path to import to, Should start with /Content.
    */
    virtual std::vector<WId> Import(const char* file_path, const char* asset_directory) = 0;

    virtual std::unique_ptr<WImporter> clone() = 0;

    virtual std::vector<std::string> Extensions() = 0;

    virtual std::vector<std::string> Formats() = 0;

protected:

    WAssetManagerFacade &ObjectManager();

private:

    TRef<WAssetManagerFacade> object_manager_{nullptr};  // TODO each WObject an unique WId (WAssetManager)
    
};

class WIMPORTERS_API WImportObj : public WImporter
{
    
public:
    
    constexpr explicit WImportObj(WAssetManagerFacade & in_object_manager) noexcept;

    virtual ~WImportObj() = default;

    constexpr WImportObj(const WImportObj & in_other) noexcept = default;

    constexpr WImportObj(WImportObj && out_other) noexcept = default;
    
    constexpr WImportObj & operator=(const WImportObj & in_other) noexcept = default;

    constexpr WImportObj & operator=(WImportObj && out_other) noexcept = default;

public:

    std::vector<WId> Import(const char * file_path, const char * asset_directory) override;

    std::unique_ptr<WImporter> clone() override;

    std::vector<std::string> Extensions() override;

    std::vector<std::string> Formats() override;
};

class WIMPORTERS_API WImportTexture : public WImporter
{
    
public:

    constexpr explicit WImportTexture(WAssetManagerFacade & in_object_manager) noexcept;

    virtual ~WImportTexture() = default;

    constexpr WImportTexture(const WImportTexture & in_other) noexcept = default;

    constexpr WImportTexture(WImportTexture && out_other) noexcept = default;
    
    constexpr WImportTexture & operator=(const WImportTexture & in_other) noexcept = default;

    constexpr WImportTexture & operator=(WImportTexture && out_other) noexcept = default;

public:
    
    std::vector<WId> Import(const char * file_path, const char * asset_directory) override;

    std::unique_ptr<WImporter> clone() override;

    std::vector<std::string> Extensions() override;

    std::vector<std::string> Formats() override;
};
