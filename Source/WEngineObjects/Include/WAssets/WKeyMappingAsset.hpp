#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WAsset.hpp"
#include "WStructs/WEngineStructs.hpp"
#include <unordered_map>

#include "WKeyMappingAsset.WEngine.hpp"

WCLASS()
class WENGINEOBJECTS_API WKeyMappingAsset : public WAsset {

    WOBJECT_BODY(WKeyMappingAsset)

public:

    WInputMapStruct & InputMap() { return input_map_; }

private:

    WInputMapStruct input_map_;

};

