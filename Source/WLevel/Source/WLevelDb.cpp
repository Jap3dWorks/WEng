#include "WLevel/WLevelDb.hpp"
#include "WLevel/WLevel.hpp"

#include <cassert>

WLevelDb::WLevelDb() :
    levels_(
        [](const WLevelId& id){
            WLevel lvl("WLevel", id);
            return lvl;},
        [](WLevel&){}
        ) {}

WLevelDb::WLevelDb(const WLevelDb & other) :
    levels_(other.levels_) {}

WLevelDb::WLevelDb(WLevelDb && other) :
    levels_(std::move(other.levels_)),
    id_pool_(std::move(other.id_pool_)) {}

WLevelDb& WLevelDb::operator=(const WLevelDb & other) {
    if (this != &other) {
        levels_.Clear();
        levels_ = other.levels_;
        id_pool_ = other.id_pool_;
    }

    return *this;
}

WLevelDb& WLevelDb::operator=(WLevelDb && other){
    if (this != &other) {
        levels_.Clear();
        
        levels_ = std::move(other.levels_);
        id_pool_ = std::move(other.id_pool_);
    }

    return *this;
}
