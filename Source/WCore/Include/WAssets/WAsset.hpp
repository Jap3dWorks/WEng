#pragma once

#include "WCore/WCore.h"
#include <string>
#include "WSerialize/ISerializable.hpp"

WCLASS()
class WCORE_API WAsset : public WObject, public ISerializable
{
    WOBJECT_BODY(WAsset)

public:

    virtual void Serialize() override;
    virtual void Deserialize() override;
};


