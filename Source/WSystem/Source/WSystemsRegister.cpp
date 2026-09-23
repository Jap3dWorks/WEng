#include "WSystem/WSystemsRegister.hpp"

wcr::wid::WSystemId WSystemsRegister::RegSystem(std::string_view in_name,
                                                const WSystemFn & in_system) {
    wcr::wid::WSystemId id = id_pool_.Generate();

    system_set_.Insert(id.GetId(), in_system);

    name_wid_[in_name] = id;

    return id;
}


