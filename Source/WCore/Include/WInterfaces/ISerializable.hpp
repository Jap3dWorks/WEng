/**
 * @file WSerialize.hpp
 * @brief Serialization classes and functions.
*/
#pragma once
#include <vector>
#include <string>
#include "WCore/WCore.hpp"

/**
 * @brief Interface for all serializable objects.
*/
class ISerializable
{
public:
    
    ISerializable() = default;
    virtual ~ISerializable() = default;

    virtual void Serialize(const std::string &) =0;
    virtual void Deserialize(const std::string &)=0;
    
};

