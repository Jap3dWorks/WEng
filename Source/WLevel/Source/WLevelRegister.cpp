#include "WLevelRegister/WLevelRegister.hpp"
#include "WEngineInterfaces/ILevel.hpp"

#include <cassert>

WLevelRegister::WLevelRegister(const WLevelRegister & other) :
    levels_(),
    id_pool_(other.id_pool_),
    current_(other.current_) {
    for (auto & p : other.levels_) {
        levels_[p.first] = p.second->Clone();
    }
}

WLevelRegister::WLevelRegister(WLevelRegister && other) :
    levels_(std::move(other.levels_)),
    id_pool_(std::move(other.id_pool_)),
    current_(std::move(other.current_))
{}

WLevelRegister& WLevelRegister::operator=(const WLevelRegister & other) {
    if (this != &other) {
        levels_.clear();

        id_pool_ = other.id_pool_;
        current_ = other.current_;

        for (auto & p : other.levels_) {
            levels_[p.first] = p.second->Clone();
        }
    }

    return *this;
}

WLevelRegister& WLevelRegister::operator=(WLevelRegister && other){
    if (this != &other) {
        levels_.clear();
        
        levels_ = std::move(other.levels_);
        id_pool_ = std::move(other.id_pool_);
        current_ = std::move(other.current_);
    }

    return *this;
}

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



