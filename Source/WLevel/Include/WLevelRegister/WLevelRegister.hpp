#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TOptionalRef.hpp"
#include "WCore/WIdPool.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WLevel/WLevel.hpp"

/**
 * @brief Register Open and Close Levels.
 */
class WLEVEL_API WLevelRegister {
public:

    using WLevelIdPool = WIdPool<WLevelId>;

    using WLevelDb = TObjectDataBase<WLevel, void, WLevelId>;

    WLevelRegister();

    virtual ~WLevelRegister() = default;

    WLevelRegister(const WLevelRegister & other);

    WLevelRegister(WLevelRegister && other);

    WLevelRegister& operator=(const WLevelRegister & other);

    WLevelRegister& operator=(WLevelRegister && other);

    WLevelId Create() {
        return levels_.Create();
    }

    bool Contains(const WLevelId & in_id) const {
        return levels_.Contains(in_id);
    }

    WLevel & Get(const WLevelId & in_id) {
        return levels_.Get(in_id);
    }

    WLevel GetCopy(const WLevelId & in_id) const {
        return levels_.Get(in_id);
    }

private:
    
    WLevelIdPool id_pool_;

    WLevelDb levels_;

};

