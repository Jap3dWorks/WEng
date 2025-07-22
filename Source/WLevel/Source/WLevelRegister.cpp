#include "WLevelRegister/WLevelRegister.hpp"
#include "WEngineInterfaces/ILevel.hpp"

#include <cassert>

WId WLevelRegister::RegisterLevel(std::unique_ptr<ILevel> && in_level) {
    WId id = id_pool_.Generate();
    levels_[id] = std::move(in_level);

    levels_[id]->WID(id);

    return id;
}

TOptionalRef<ILevel> WLevelRegister::Get(const WId & in_id) {
    assert(levels_.contains(in_id));
    return levels_.at(in_id).get();
}



