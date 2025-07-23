#include "WLevelRegister/WLevelRegister.hpp"
#include "WLevel/WLevel.hpp"

#include <cassert>

WLevelRegister::WLevelRegister() :
    levels_([](const WId& id){ WLevel lvl("WLevel"); lvl.WID(id); return lvl;},
            [](WLevel&){}),
    current_() {}


WLevelRegister::WLevelRegister(const WLevelRegister & other) :
    levels_(other.levels_),
    current_(other.current_) {}

WLevelRegister::WLevelRegister(WLevelRegister && other) :
    levels_(std::move(other.levels_)),
    id_pool_(std::move(other.id_pool_)),
    current_(std::move(other.current_))
{}

WLevelRegister& WLevelRegister::operator=(const WLevelRegister & other) {
    if (this != &other) {
        levels_.Clear();
        levels_ = other.levels_;
        id_pool_ = other.id_pool_;
        current_ = other.current_;
    }

    return *this;
}

WLevelRegister& WLevelRegister::operator=(WLevelRegister && other){
    if (this != &other) {
        levels_.Clear();
        
        levels_ = std::move(other.levels_);
        id_pool_ = std::move(other.id_pool_);
        current_ = std::move(other.current_);
    }

    return *this;
}

WLevel & WLevelRegister::Create() {
    WId id =  levels_.Create();
    return levels_.Get(id);
}

TOptionalRef<WLevel> WLevelRegister::Get(const WId & in_id) {
    assert(levels_.contains(in_id));
    return levels_.Get(in_id);
}



