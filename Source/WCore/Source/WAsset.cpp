#include "WAsset.h"


// WAsset
// ------

void WAsset::SetName(const char* name)
{
    name_ = name;
}

void WAsset::SetPath(const char* path)
{
    path_ = path;
}

std::string WAsset::GetName() const
{
    return name_;
}

std::string WAsset::GetPath() const
{
    return path_;
}