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

    void SetFilePath(const std::string& file_path);
    std::string GetFilePath() const;

protected:
    std::vector<char> data_{};
    std::string file_path_{};
    std::vector<WObject*> objects_{};
};

