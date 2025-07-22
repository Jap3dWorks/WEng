#include "WLevelRegister/WLevelRegister.hpp"
#include "WEngineInterfaces/ILevel.hpp"

#include <cassert>

WId WLevelRegister::RegisterLevel(std::unique_ptr<ILevel> && in_level) {
    WId id = id_pool_.Generate();
    levels_[id] = std::move(in_level);

    levels_[id]->WID(id);

    return id;
}

bool WLevelRegister::LoadLevel(const WId & in_id) {
    return true;
}

WId WLevelRegister::Current() const {
    return current_;
}

void WLevelRegister::ApplyCurrent(const TFunction<void(ILevel*)> & in_fn) const {
    assert(current_);
    in_fn(levels_.at(current_).get());
}

void WLevelRegister::Apply(const WId & in_id, const TFunction<void(ILevel*)> & in_fn) const {
    assert(levels_.contains(in_id));
    in_fn(levels_.at(in_id).get());
}

TOptionalRef<ILevel> WLevelRegister::Get(const WId & in_id) {
    assert(levels_.contains(in_id));
    return levels_.at(in_id).get();
}



