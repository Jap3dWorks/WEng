#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TOptionalRef.hpp"
#include "WCore/WIdPool.hpp"
#include "WCore/TObjectDataBase.hpp"


#include <memory>

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

    WId RegisterLevel(std::unique_ptr<WLevel> && in_level);

    TOptionalRef<WLevel> Get(const WId & in_id);

private:
    
    WIdPool id_pool_;

    WId current_;

    TObjectDataBase<WLevel> levels_;

    // std::unordered_map<WId, std::unique_ptr<ILevel>> levels_;

};

