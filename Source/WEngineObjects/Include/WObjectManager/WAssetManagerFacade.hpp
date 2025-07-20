#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TRef.hpp"
#include "WCore/WIdPool.hpp"
#include "WObjectManager/WObjectManager.hpp"

#include <unordered_map>

class WAsset;

/**
 * @brief WRapper class for a WObjectManager specialized in the asset management
 */
class WENGINEOBJECTS_API WAssetManagerFacade {
public:

    WAssetManagerFacade(WObjectManager & in_object_manager) :
    object_manager_(in_object_manager),
    id_pool_(),
    id_class_() {
        InitializeIdPool();
    }

    template<std::derived_from<WObject> T>
    WId Create(const char * in_fullname) {}

    WId Create(const WClass * in_class, const char * in_fullname);

    TWRef<WAsset> Get(const WId & in_id) const;

    TRef<WObjectManager> ObjectManager() noexcept;

    const TRef<WObjectManager> ObjectManager() const noexcept;

private:

    void InitializeIdPool();

    TRef<WObjectManager> object_manager_;

    WIdPool id_pool_;

    std::unordered_map<WId, const WClass *> id_class_;
    
};
