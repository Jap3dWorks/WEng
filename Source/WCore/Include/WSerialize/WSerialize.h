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
    
    void SetPath(const std::string& file_path);
    std::string GetPath() const;

private:
    // Game based path
    std::string file_path_{};
};

