#pragma once

#include "WCore.h"
#include "Geometry/WGeometryTypes.h"
#include "Transforms/WTransforms.h"

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
    WTransform transform{};
};

WCLASS()
class WCORE_API WStaticModelComponent : public WComponent
{
    WOBJECT_BODY(WStaticModelComponent)
public:
    WModel* model=nullptr;
};
