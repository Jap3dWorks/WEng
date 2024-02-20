#include "WSerialize/WSerialize.h"


void WSerializable::SetPath(const std::string& file_path)
{
    file_path_ = file_path;
}

std::string WSerializable::GetPath() const
{
    return file_path_;
}
