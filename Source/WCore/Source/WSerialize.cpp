#include "WSerialize/WSerialize.h"


void ISerializable::SetPath(const std::string& file_path)
{
    file_path_ = file_path;
}

std::string ISerializable::GetPath() const
{
    return file_path_;
}
