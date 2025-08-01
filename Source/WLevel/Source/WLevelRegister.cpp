#include "WLevelRegister/WLevelRegister.hpp"
#include "WLevel/WLevel.hpp"

#include <cassert>

WLevelRegister::WLevelRegister() :
    levels_(
        [](const WLevelId& id){
            WLevel lvl("WLevel", id);
            return lvl;},
        [](WLevel&){}
        ) {}

WLevelRegister::WLevelRegister(const WLevelRegister & other) :
    levels_(other.levels_) {}

WLevelRegister::WLevelRegister(WLevelRegister && other) :
    levels_(std::move(other.levels_)),
    id_pool_(std::move(other.id_pool_)) {}

WLevelRegister& WLevelRegister::operator=(const WLevelRegister & other) {
    if (this != &other) {
        levels_.Clear();
        levels_ = other.levels_;
        id_pool_ = other.id_pool_;
    }

    return *this;
}

WLevelRegister& WLevelRegister::operator=(WLevelRegister && other){
    if (this != &other) {
        levels_.Clear();
        
        levels_ = std::move(other.levels_);
        id_pool_ = std::move(other.id_pool_);
    }

    return *this;
}

WLevelId WLevelRegister::Create() {
    return levels_.Create();
}

TOptionalRef<WLevel> WLevelRegister::Get(const WLevelId & in_id) {
    assert(levels_.Contains(in_id));
    return levels_.Get(in_id);
}

WLevel WLevelRegister::GetCopy(const WLevelId & in_id) const {
    return levels_.Get(in_id);
}
