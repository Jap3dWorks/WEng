#include "WObjectDb/WAssetDb.hpp"
#include "WEngineObjects/WAsset.hpp"

#include <cassert>

void WAssetDb::InitializeIdPool() {
    for(const WClass * c : object_manager_.IterWClasses()) {
        for (WAssetId wid : object_manager_.Indexes(c)) {
            id_pool_.Reserve(wid);
            id_class_[wid] = c;
        }
    }
}
