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

    WLevelRegister();

    virtual ~WLevelRegister() = default;

    WLevelRegister(const WLevelRegister & other);

    WLevelRegister(WLevelRegister && other);

    WLevelRegister& operator=(const WLevelRegister & other);

    WLevelRegister& operator=(WLevelRegister && other);

    WId Create();    

    TOptionalRef<WLevel> Get(const WId & in_id);

    WLevel GetCopy(const WId & in_id) const;

private:
    
    WIdPool id_pool_;

    TObjectDataBase<WLevel> levels_;

};

