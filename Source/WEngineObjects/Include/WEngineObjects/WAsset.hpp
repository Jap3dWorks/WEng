#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WObject.hpp"
#include "WInterfaces/ISerializable.hpp"
#include "WEngineObjects/WClass.hpp"

#include <string>

#include "WAsset.WEngine.hpp"

WCLASS()
class WCORE_API WAsset : public WObject, public ISerializable
{
    WOBJECT_BODY(WAsset)

public:

    void Serialize(const std::string & in_path) override;
    void Deserialize(const std::string & in_path) override;
    
};


