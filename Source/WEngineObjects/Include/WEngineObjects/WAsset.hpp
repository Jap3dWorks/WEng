#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WObject.hpp"
#include "WInterfaces/ISerializable.hpp"
#include "WClass/WClass.hpp"

#include <string>

#include "WAsset.WEng.hpp"

class WCORE_API WAsset : public WObject, public ISerializable
{
    
    WOBJECT_BODY;

public:

    WPROPERTY(WObjectName, name,);
    WPROPERTY(wcr::wid::WAssetId, asset_id,);  // TODO add typeindex

public:

    void Serialize(const std::string & in_path) override;

    void Deserialize(const std::string & in_path) override;

private:

};


