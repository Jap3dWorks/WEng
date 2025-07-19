#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TRef.hpp"
#include "WCore/WIdPool.hpp"

#include "WObjectManager/WObjectManager.hpp"

class WAsset;

/**
 * @brief WRapper class for a WObjectManager specialized in the asset management
 */
class WENGINEOBJECTS_API WAssetManager {
public:

    WAssetManager(WObjectManager & in_object_manager) :
    object_manager_(in_object_manager) {
        InitializeIdPool();
    }

    template<std::derived_from<WObject> T>
    WId Create() {}

    WId Create(const WClass * in_class);

    TWRef<WAsset> Get() const;

    const TRef<WObjectManager> ObjectManager() const;

private:

    void InitializeIdPool();

    TRef<WObjectManager> object_manager_;
    WIdPool id_pool_;
};
