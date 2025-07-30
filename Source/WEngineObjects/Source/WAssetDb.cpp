#include "WObjectDb/WAssetDb.hpp"
#include "WEngineObjects/WAsset.hpp"

#include <cassert>

void WAssetDb::InitializeIdPool() {
    for(auto c : object_manager_.Classes()) {
        for (WAssetId wid : object_manager_.Indexes(c)) {
            id_pool_.Reserve(wid);
            id_class_[wid] = c;
        }
    }
}
