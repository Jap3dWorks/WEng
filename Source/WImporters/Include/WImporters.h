#pragma once

#include <vector>
#include <memory>

#include "WCore/WCore.h"
#include "WCore/ICloneable.h"
#include "WCore/TRef.h"

class WAsset;
class WObjectManager;

class WIMPORTERS_API WImporter : public ICloneable<WImporter>
{
public:

    WImporter() = default;
    virtual ~WImporter() = default;

    WImporter(const WImporter & in_other) = default;
    WImporter(WImporter && in_other) = default;

    WImporter& operator=(const WImporter & in_other) = default;
    WImporter& operator=(WImporter && in_other) = default;

    /**
     * @brief Import an asset from a file
     * @param file_path The file path to import
     * @param asset_path The asset path to import to, relative to the Content directory
    */
    virtual std::vector<WAsset*> Import(const char* file_path, const char* asset_path) = 0;

public:    

    virtual std::unique_ptr<WImporter> clone() = 0;

protected:

    virtual std::vector<std::string> Extensions() = 0;
    virtual std::vector<std::string> Formats() = 0;

private:

    TRef<WObjectManager> object_manager_{nullptr};
    
};

class WIMPORTERS_API WImportObj : public WImporter
{
    
public:

    std::vector<WAsset*> Import(const char* file_path, const char* asset_path) override;

    std::unique_ptr<WImporter> clone() override;

protected:
    
    virtual std::vector<std::string> Extensions() override;
    virtual std::vector<std::string> Formats() override;
};

class WIMPORTERS_API WImportTexture : public WImporter
{
    
public:

    std::vector<WAsset*> Import(const char* file_path, const char* asset_path) override;

    std::unique_ptr<WImporter> clone() override;

protected:
    
    virtual std::vector<std::string> Extensions() override;
    virtual std::vector<std::string> Formats() override;
};
