#include "WObjectManager/WAssetManagerFacade.hpp"



void WAssetManagerFacade::InitializeIdPool() {
    for(auto c : object_manager_->Classes()) {
        for (WId wid : object_manager_->Indexes(c)) {
            id_pool_.Reserve(wid);
        }
    }
}
