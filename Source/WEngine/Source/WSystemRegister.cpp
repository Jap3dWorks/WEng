#include "WSystems/WSystemRegister.hpp"

WSystemRegister::WSystemRegister() :
    level_system_set_(),
    system_set_(),
    id_pool_()
{
    
}

WSystemId WSystemRegister::RegLevelSystem(const char * in_name, const WLevelSystemFn & in_level_system)
{
    WSystemId id = id_pool_.Generate();
    level_system_set_.Insert(id.GetId(), in_level_system);

    name_wid_[in_name] = id;

    return id;
}

WSystemId WSystemRegister::RegSystem(const char * in_name, const WSystemFn & in_system) {
    WSystemId id = id_pool_.Generate();
    system_set_.Insert(id.GetId(), in_system);

    name_wid_[in_name] = id;

    return id;
}


