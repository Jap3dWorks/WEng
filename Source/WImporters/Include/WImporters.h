#pragma once

#include "WCore/WCore.h"
#include <vector>

class WAsset;

class WIMPORTERS_API WImporter
{
public:
    WImporter() = default;
    virtual ~WImporter() = default;

    /**
     * @brief Import an asset from a file
     * @param file_path The file path to import
     * @param asset_path The asset path to import to, relative to the Content directory
    */
    virtual std::vector<WAsset*> Import(const char* file_path, const char* asset_path) = 0;

protected:

    virtual std::vector<std::string> Extensions() = 0;
    virtual std::vector<std::string> Formats() = 0;
};

class WIMPORTERS_API WImportObj : public WImporter
{
public:
    WImportObj() = default;
    virtual ~WImportObj() = default;

    std::vector<WAsset*> Import(const char* file_path, const char* asset_path) override;

protected:
    virtual std::vector<std::string> Extensions() override;
    virtual std::vector<std::string> Formats() override;
};

class WIMPORTERS_API WImportTexture : public WImporter
{
public:
    WImportTexture() = default;
    virtual ~WImportTexture() = default;

    std::vector<WAsset*> Import(const char* file_path, const char* asset_path) override;

protected:
    virtual std::vector<std::string> Extensions() override;
    virtual std::vector<std::string> Formats() override;
};
