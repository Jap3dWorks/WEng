#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WAsset.hpp"
#include "WCoreTypes/WEngineStructs.hpp"
#include <unordered_map>

#include "WInputMappingAsset.WEngine.hpp"

using WInputMap = std::unordered_map<WInput, std::vector<wid::WAssetId>>;

class WENGINEOBJECTS_API WInputMappingAsset : public WAsset {

    WOBJECT_BODY

public:

    WPROPERTY(WInputMap, input_map, );

};

