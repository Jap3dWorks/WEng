#pragma once

#include "WCore.h"
#include "WAssets/WAsset.h"
#include "WGeometry/WGeometryTypes.h"


WCLASS()
class WCORE_API WStaticModel : public WAsset
{
    WOBJECT_BODY(WStaticModel)
public:
    WModel model_{};
};