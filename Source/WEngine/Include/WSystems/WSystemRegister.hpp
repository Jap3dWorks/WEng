#pragma once

#include "WCore/WCore.hpp"
#include "WSystems/WSystems.hpp"
#include "WCore/TSparseSet.hpp"
#include "WCore/WIdPool.hpp"

#include <unordered_map>
#include <string>

// TODO All WSystems registry, active or not. this will be useful for Serialization.

class WENGINE_API WSystemRegister {

    WSystemRegister();

    WSystemId RegLevelSystem(const char * in_name, const WLevelSystemFn & in_level_system);

    WSystemId RegSystem(const char * in_name, const WSystemFn & in_system);

private:

    TSparseSet<WLevelSystemFn> level_system_set_;
    TSparseSet<WSystemFn> system_set_;

    WIdPool<WSystemId> id_pool_;
    std::unordered_map<std::string, WSystemId> name_wid_;

};


