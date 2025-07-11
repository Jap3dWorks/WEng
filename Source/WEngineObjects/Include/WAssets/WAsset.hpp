#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WObject.hpp"
#include "WInterfaces/ISerializable.hpp"

#include <string>

WCLASS()
class WCORE_API WAsset : public WObject, public ISerializable
{
    WOBJECT_BODY(WAsset)

public:

    virtual void Serialize() override;
    virtual void Deserialize() override;
};


