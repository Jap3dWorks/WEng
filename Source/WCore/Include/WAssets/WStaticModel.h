#pragma once

#include "WCore/WCore.h"
#include "WAssets/WAsset.h"
#include "WGeometry/WGeometryTypes.h"


WCLASS()
class WCORE_API WStaticModel : public WAsset
{
    WOBJECT_BODY(WStaticModel)
public:
    void SetModel(const WModel& model);

private:
    WModel model_{};
};