#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TFunction.hpp"

/**
 * To declare systems some Macros are required to be defined
 * WSYSTEM_MODULE_NAME
 * e.g #define WSYSTEM_MODULE_NAME WSYSTEM
 *
 * WSYSTEM_MODULE_NAMESPACE
 * e.g #define WSYSTEM_MODULE_NAMESPACE wsm
 *
 * WSYSTEM_COLLECTION
 * e.g #define WSYSTEM_COLLECTION common
 *
 * WSYSTEM_XITEMS WSYSTEM_X(SystemName1) ...
 * e.g #define WSYSTEM_XITEMS WSYSTEM_X(ExampleSystemName1)    \
 *             WSYSTEM_X(ExampleSystemName2)                   \
 *             ...
 *
 * Also it is required to include after the upper definitions the following file
 * #include "WSystem/Boilerplate/WSYSTEM_DECLARE.inc"
 *
 * Once all is setup systems definitions can be done in .cpp files. The generated symbol name is 
 * bool WSYSTEM_MODULE_NAMESPACE::system::WSYSTEM_COLLECTION::<SystemName>(wsm::SystemParameters parameters) { ... }
 *
 * e.g bool wsm::system::common::ExampleSystemName1(wsm::SystemParameters parameters) { return true; }
 */

class WEngine;
namespace was { class Level; }

namespace wsm {

    struct SystemParameters {
        WEngine * engine;
        was::Level * level;
    };

    using SystemFn = TFnPtr<bool(wsm::SystemParameters const &)>;

}

