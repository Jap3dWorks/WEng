#include "WSerialize/WSerialize.h"


void WSerializable::set_file_path(const std::string& file_path)
{
    file_path_ = file_path;
}

std::string WSerializable::get_file_path() const
{
    return file_path_;
}
