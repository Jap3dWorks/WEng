// -*- mode: C++ -*-

#include "WSystem/WSystem.hpp"
#include "WSystem/SystemRegister.hpp"
#include <array>

#ifdef JOIN_1
#undef JOIN_1
#endif
#define JOIN_1(_X,_Y) _X ## _Y

#ifdef JOIN
#undef JOIN
#endif
#define JOIN(_X,_Y) JOIN_1(_X,_Y)

#define WSYSYEM_TOTAL_NAMESPACE WSYSTEM_MODULE_NAMESPACE :: system :: WSYSTEM_COLLECTION
#define WSYSTEM_MODULE_API JOIN(WSYSTEM_MODULE_NAME, _API)

namespace WSYSYEM_TOTAL_NAMESPACE {

#define TEST WSYSTEM_MODULE_API

#define WSYSTEM_X(Name) \
    bool WSYSTEM_MODULE_API Name(wsm::SystemParameters const &); \

    WSYSTEM_XITEMS

#undef WSYSTEM_X


#define WSYSTEM_X(Name) #Name ,

    inline constexpr std::array const SYSTEM_NAMES {WSYSTEM_XITEMS ""};

#undef WSYSTEM_X

#define WSYSTEM_X(Name) constexpr std::string_view const Name ## _str {#Name}; \

WSYSTEM_XITEMS    

#define WSYSTEM_X(Name) reg.RegSystem(Name ## _str, Name); \
    
    inline constexpr void RegSystems(wsm::SystemsRegister & reg) {
        WSYSTEM_XITEMS ;
    }

}

#undef WSYSTEM_X
#undef WSYSTEM_XITEMS
#undef WSYSTEM_MODULE_NAME
#undef WSYSTEM_COLLECTION
#undef WSYSTEM_MODULE_API
#undef WSYSTEM_MODULE_NAMESPACE
#undef WSYSYEM_TOTAL_NAMESPACE
#undef JOIN_1
#undef JOIN
