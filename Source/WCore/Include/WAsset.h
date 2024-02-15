#pragma once

#include "WCore.h"
#include <string>


class WCORE_API WAsset : public WObject
{
public:
    WAsset() = default;
    virtual ~WAsset() = default;

    virtual void Load(const char* file_path) = 0;
    virtual void Save(const char* file_path) = 0;

    virtual void SetName(const char* name);
    virtual void SetPath(const char* path);

    virtual std::string GetName() const;
    virtual std::string GetPath() const;

private:
    std::string name_; // name of the asset
    std::string path_; // path to the asset
};

