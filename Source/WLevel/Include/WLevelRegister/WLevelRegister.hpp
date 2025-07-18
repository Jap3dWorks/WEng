#pragma once

#include "WCore/WCore.hpp"
#include "ILevelRegister.hpp"
#include "WCore/WIdPool.hpp"

#include <unordered_map>
#include <memory>

/**
 * @brief 
 */
class WLEVEL_API WLevelRegister: public ILevelRegister {
public:

    WLevelRegister() = default;
    ~WLevelRegister() override = default;

    WId RegisterLevel(std::unique_ptr<ILevelRegister> && in_level) {
        WId id = id_pool_.Generate();
        levels_[id] = std::move(in_level);

        return id;
    }

private:
    
    WIdPool id_pool_;

    WId current_;

    std::unordered_map<WId, std::unique_ptr<ILevelRegister>> levels_;

};
