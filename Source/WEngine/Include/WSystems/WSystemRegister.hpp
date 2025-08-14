#pragma once

#include "WCore/WCore.hpp"
#include "WSystems/WSystems.hpp"

// TODO All WSystems registry, active or not. this will be useful for Serialization.

class WENGINE_API WSystemRegister {

    void RegLevelSystem(const char * in_name, const WLevelSystemFn * in_level_system);

    void RegSystem(const char * in_name, const WSystemFn * in_system);
  
};


