#pragma once

#include "WSystem/WSystem.hpp"

#define WSYSTEM_MODULE_NAME WCOLLISION
#define WSYSTEM_MODULE_NAMESPACE wcl
#define WSYSTEM_COLLECTION collision

#define WSYSTEM_XITEMS WSYSTEM_X(INIT_SetupLevelCollision)  \
    WSYSTEM_X(PRE_CheckCollision)                           \
    WSYSTEM_X(MID_TriggerCollisionEvents)                   \
    WSYSTEM_X(END_CleanCollisionData)

#include "WSystem/Boilerplate/WSYSTEM_DECLARE.inc"

