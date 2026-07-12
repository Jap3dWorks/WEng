#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WObject.hpp"
#include "WInterfaces/ISerializable.hpp"
#include "WClass/WClass.hpp"

#include <string>

#include "WAsset.WEngine.hpp"

class WCORE_API WAsset : public WObject, public ISerializable
{
    
    WOBJECT_BODY;

public:

    WPROPERTY(WObjectName, name,);
    WPROPERTY(wid::WAssetId, asset_id,);

public:

    void Serialize(const std::string & in_path) override;

    void Deserialize(const std::string & in_path) override;

private:

};


