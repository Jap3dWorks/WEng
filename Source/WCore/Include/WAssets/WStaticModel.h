#pragma once

#include "WCore/WCore.hpp"
#include "WAssets/WAsset.hpp"
#include "WStructs/WGeometryStructs.h"


WCLASS()
class WCORE_API WStaticModel : public WAsset
{
    WOBJECT_BODY(WStaticModel)

public:
    void SetModel(const WModelStruct& model);
    const WModelStruct& GetModel() const;

private:
    WModelStruct model_{};
};