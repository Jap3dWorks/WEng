#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TOptionalRef.hpp"
#include "WCore/WIdPool.hpp"
#include "WCore/TObjectDataBase.hpp"

class WLevel;

/**
 * @brief Register Open and Close Levels.
 */
class WLEVEL_API WLevelRegister {
public:

    using WLevelIdPool = WIdPool<WAssetId>;

    using WLevelDb = TObjectDataBase<WLevel, void, WAssetId>;

    WLevelRegister();

    virtual ~WLevelRegister() = default;

    WLevelRegister(const WLevelRegister & other);

    WLevelRegister(WLevelRegister && other);

    WLevelRegister& operator=(const WLevelRegister & other);

    WLevelRegister& operator=(WLevelRegister && other);

    WAssetId Create();

    TOptionalRef<WLevel> Get(const WAssetId & in_id);

    WLevel GetCopy(const WAssetId & in_id) const;

private:
    
    WLevelIdPool id_pool_;

    WLevelDb levels_;

};

