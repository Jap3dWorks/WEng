#pragma once

#include "WCore/WCore.hpp"
#include "WObjects/WAsset.hpp"
#include "WCoreTypes/WEngineStructs.hpp"
#include <unordered_map>

#include "was::InputMapping.WEng.hpp"

namespace was {
    
    using InputMap = std::unordered_map<WInput, std::vector<wcr::wid::WAssetId>>;

    class WOBJECTS_API InputMapping : public WAsset {

        WOBJECT_BODY;

    public:

        WPROPERTY(InputMap, input_map, );

    };

}
