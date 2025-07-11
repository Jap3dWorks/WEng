#pragma once

#include "WCore/WCore.hpp"
#include "WInterfaces/ICloneable.hpp"
#include "WCore/TRef.hpp"
#include "WEngineObjects/TWRef.hpp"
#include "WAssets/WAsset.hpp"

#include <vector>
#include <memory>

class WObjectManager;

class WIMPORTERS_API WImporter : public ICloneable<WImporter>
{
public:

    explicit WImporter(WObjectManager & in_object_manager);
    virtual ~WImporter() = default;

    WImporter(const WImporter & in_other);
    WImporter(WImporter && out_other) noexcept;

    WImporter & operator=(const WImporter & in_other);
    WImporter & operator=(WImporter && out_other) noexcept;

    /**
     * @brief Import an asset from a file
     * @param file_path The file path to import
     * @param asset_directory, Engine based directory path to import to, Should start with /Content.
    */
    virtual std::vector<TWRef<WAsset>> Import(const char* file_path, const char* asset_directory) = 0;

public:    

    virtual std::unique_ptr<WImporter> clone() = 0;
    virtual std::vector<std::string> Extensions() = 0;
    virtual std::vector<std::string> Formats() = 0;

protected:

    WObjectManager & ObjectManager();

private:

    void Move(WImporter && other) noexcept;

    TRef<WObjectManager> object_manager_{nullptr};
    
};

class WIMPORTERS_API WImportObj : public WImporter
{
    
public:
    
    explicit WImportObj(WObjectManager & in_object_manager);

    virtual ~WImportObj() = default;

    WImportObj(const WImportObj & in_other) = default;
    WImportObj(WImportObj && out_other) noexcept = default;
    
    WImportObj & operator=(const WImportObj & in_other) = default;
    WImportObj & operator=(WImportObj && out_other) noexcept = default;

    std::vector<TWRef<WAsset>> Import(const char * file_path, const char * asset_directory) override;

    std::unique_ptr<WImporter> clone() override;

    virtual std::vector<std::string> Extensions() override;
    virtual std::vector<std::string> Formats() override;
};

class WIMPORTERS_API WImportTexture : public WImporter
{
    
public:

    explicit WImportTexture(WObjectManager & in_object_manager);

    virtual ~WImportTexture() = default;

    WImportTexture(const WImportTexture & in_other) = default;
    WImportTexture(WImportTexture && out_other) noexcept = default;
    
    WImportTexture & operator=(const WImportTexture & in_other) = default;
    WImportTexture & operator=(WImportTexture && out_other) noexcept = default;

    std::vector<TWRef<WAsset>> Import(const char * file_path, const char * asset_directory) override;

    std::unique_ptr<WImporter> clone() override;

    virtual std::vector<std::string> Extensions() override;
    virtual std::vector<std::string> Formats() override;
};
