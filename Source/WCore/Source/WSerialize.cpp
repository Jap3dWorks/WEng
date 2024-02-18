#include "WSerialize/WSerialize.h"


void WSerializable::SetFilePath(const std::string& file_path)
{
    file_path_ = file_path;
}

std::string WSerializable::GetFilePath() const
{
    return file_path_;
}
