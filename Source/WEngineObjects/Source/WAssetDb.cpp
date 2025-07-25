#include "WObjectDb/WAssetDb.hpp"
#include "WEngineObjects/WAsset.hpp"

#include <cassert>

void WAssetDb::InitializeIdPool() {
    for(auto c : object_manager_.Classes()) {
        for (WId wid : object_manager_.Indexes(c)) {
            id_pool_.Reserve(wid);
            id_class_[wid] = c;
        }
    }
}

// WId WAssetDb::Create(const WClass * in_class, const char * in_fullname) {
//     WId id = id_pool_.Generate();
//     id_class_[id] = in_class;
    
//     object_manager_.Create(in_class, id, in_fullname);

//     return id;
// }

// TWRef<WAsset> WAssetDb::Get(const WId & in_id) {
//     assert(id_class_.contains(in_id));
    
//     return static_cast<WAsset*>(object_manager_.Get(id_class_.at(in_id), in_id).Ptr());
// }

// TRef<WObjectDb> WAssetDb::ObjectManager() noexcept {
//     return object_manager_;
// }

