#include "WSystems/WSystemsRegister.hpp"

WSystemsRegister::WSystemsRegister() :
    system_set_(),
    id_pool_()
{
}

WSystemId WSystemsRegister::RegSystem(const char * in_name, const WSystemFn & in_system) {
    WSystemId id = id_pool_.Generate();

    system_set_.Insert(id.GetId(), in_system);

    name_wid_[in_name] = id;

    return id;
}


