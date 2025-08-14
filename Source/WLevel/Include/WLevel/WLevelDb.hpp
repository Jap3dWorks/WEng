#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TOptionalRef.hpp"
#include "WCore/WIdPool.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WLevel/WLevel.hpp"

/**
 * @brief Levels Db
 */
class WLEVEL_API WLevelDb {
public:

    using WLevelIdPool = WIdPool<WLevelId>;

    using WLevelDbType = TObjectDataBase<WLevel, WLevel, WLevelId>;

public:

    WLevelDb();

    virtual ~WLevelDb() = default;

    WLevelDb(const WLevelDb & other);

    WLevelDb(WLevelDb && other);

    WLevelDb& operator=(const WLevelDb & other);

    WLevelDb& operator=(WLevelDb && other);

    WLevelId Create() {
        return levels_.Create();
    }

    bool Contains(const WLevelId & in_id) const {
        return levels_.Contains(in_id);
    }

    WLevel & Get(const WLevelId & in_id) {
        return levels_.Get(in_id);
    }

    const WLevel & Get(const WLevelId & in_id) const {
        return levels_.Get(in_id);
    }

private:
    
    WLevelIdPool id_pool_;

    WLevelDbType levels_;

};

