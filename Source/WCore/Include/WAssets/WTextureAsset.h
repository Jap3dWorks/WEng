#pragma once

#include "WCore/WCore.h"
#include "WAssets/WAsset.h"
#include "WGeometry/WGeometryTypes.h"
#include <memory>


WCLASS()
class WCORE_API WTextureAsset : public WAsset
{
    WOBJECT_BODY(WTextureAsset)

    public:
        void SetTexture(const WTexture& texture);
        const WTexture& GetTexture() const;

    private:
        WTexture texture_{};

};