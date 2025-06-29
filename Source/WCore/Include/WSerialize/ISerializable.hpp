/**
 * @file WSerialize.h
 * @brief Serialization classes and functions.
*/
#pragma once
#include <vector>
#include <string>
#include "WCore/WCore.h"

/**
 * @brief Interface for all serializable objects.
*/
class ISerializable
{
public:
    ISerializable() = default;
    virtual ~ISerializable() = default;

    virtual void Serialize() {} // TODO make abstract
    virtual void Deserialize() {}
    
    void SetPath(const std::string& file_path);
    std::string GetPath() const;

private:
    // Game based path
    std::string file_path_{};
};

