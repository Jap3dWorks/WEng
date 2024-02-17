/**
 * @file WAsset.h
 * @brief Base class for all assets, Assets are serialized objects that are used in the engine.
 * 
 */

#pragma once

#include "WCore.h"
#include <string>
#include "WSerialize/WSerialize.h"

WCLASS()
class WCORE_API WAsset : public WObject, public WSerializable
{
    WOBJECT_BODY(WAsset)

public:

    virtual void Load(const char* file_path){}
    virtual void Save(const char* file_path){}

    virtual void SetName(const char* name);
    virtual void SetPath(const char* path);

    virtual std::string GetName() const;
    virtual std::string GetPath() const;

    virtual void Serialize() override;
    virtual void Deserialize() override;
    
private:
    std::string name_{}; // name of the asset
    std::string path_{}; // path to the asset
};


