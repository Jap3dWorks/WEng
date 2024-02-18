/**
 * @file WSerialize.h
 * @brief Serialization classes and functions.
*/
#pragma once
#include <vector>
#include <string>
#include "WCore/WCore.h"

/**
 * @brief Base class for all serializable objects.
*/
class WSerializable
{
public:
    WSerializable() = default;
    virtual ~WSerializable() = default;

    virtual void Serialize() = 0;
    virtual void Deserialize() = 0;

    void set_file_path(const std::string& file_path);
    std::string get_file_path() const;

protected:
    std::vector<char> data_{};
    std::string file_path_{};
    std::vector<WObject*> objects_{};
};

