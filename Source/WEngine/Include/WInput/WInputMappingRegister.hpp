#pragma once

#include "WCore/WCore.hpp"
#include "WStructs/WEngineStructs.hpp"

#include <unordered_map>
#include <vector>

class WENGINE_API WInputMappingRegister {

public:



private:

    // WInputMappingAsset dort by order.
    using PType = std::vector<WAssetId>;

     // Each Action Which Callables Triggers.
    // using ActionBind = std::unordered_map<WAssetId, std::vector<Binds>>;
    
};
