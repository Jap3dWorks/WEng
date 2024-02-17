#pragma once

#include "WCore.h"
#include "Geometry/WGeometryTypes.h"
#include "Transforms/WTransforms.h"

class WCORE_API WComponent : public WObject
{
};

class WCORE_API WTransformComponent : public WComponent
{   
public:
    WTransform transform{};

    WTransformComponent() = default;
    virtual ~WTransformComponent() = default;
};

class WCORE_API WStaticModelComponent : public WComponent
{
public:
    WModel* model=nullptr;
    
    WStaticModelComponent() = default;
    virtual ~WStaticModelComponent() = default;
};
