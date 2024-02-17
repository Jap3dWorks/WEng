#pragma once

#include "WCore.h"
#include <string>

WCLASS()
class WCORE_API WAsset : public WObject
{
    WOBJECT_BODY(WAsset)

public:

    virtual void Load(const char* file_path){}
    virtual void Save(const char* file_path){}

    virtual void SetName(const char* name);
    virtual void SetPath(const char* path);

    virtual std::string GetName() const;
    virtual std::string GetPath() const;

private:
    std::string name_{}; // name of the asset
    std::string path_{}; // path to the asset
};

