#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WAsset.hpp"
#include "WCoreTypes/WEngineStructs.hpp"
#include <unordered_map>

#include "WInputMappingAsset.WEngine.hpp"

WCLASS()
class WENGINEOBJECTS_API WInputMappingAsset : public WAsset {

    WOBJECT_BODY(WInputMappingAsset)

public:

    WInputMapStruct & InputMap() { return input_map_; }

private:

    WInputMapStruct input_map_;

};

