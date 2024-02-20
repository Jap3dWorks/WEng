#pragma once

#include "WCore/WCore.h"
#include "WStructs/WGeometryStructs.h"
#include "WStructs/WTransformStructs.h"

WCLASS()
class WCORE_API WComponent : public WObject
{
    WOBJECT_BODY(WComponent)
};

WCLASS()
class WCORE_API WTransformComponent : public WComponent
{
    WOBJECT_BODY(WTransformComponent)
public:
    WTransformStruct transform{};
};

WCLASS()
class WCORE_API WStaticModelComponent : public WComponent
{
    WOBJECT_BODY(WStaticModelComponent)
public:
    WModelStruct* model=nullptr;
};
