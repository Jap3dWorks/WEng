#pragma once

#include "WCore/WCore.hpp"
#include <string>
#include "WInterfaces/ISerializable.hpp"

WCLASS()
class WCORE_API WAsset : public WObject, public ISerializable
{
    WOBJECT_BODY(WAsset)

public:

    virtual void Serialize() override;
    virtual void Deserialize() override;
};


