#pragma once

#define WSYSTEM_MODULE_NAME WENGINE
#define WSYSTEM_MODULE_NAMESPACE weng
#define WSYSTEM_COLLECTION common

#define WSYSTEM_XITEMS                                \
    WSYSTEM_X(Init_InitializeTransformsMatrix)        \
    WSYSTEM_X(Init_RenderLevelResources)              \
    WSYSTEM_X(Init_CameraInput)                       \
    WSYSTEM_X(Pre_UpdateMovement)                     \
    WSYSTEM_X(Pre_CameraInputMovement)                \
    WSYSTEM_X(Post_UpdateRenderCamera)                \
    WSYSTEM_X(Post_UpdateShadowMap)                   \
    WSYSTEM_X(End_RenderLevelResources)

#include "WSystem/Boilerplate/WSYSTEM_DECLARE.inc"

