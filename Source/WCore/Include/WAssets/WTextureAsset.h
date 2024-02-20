#pragma once

#include "WCore/WCore.h"
#include "WAssets/WAsset.h"
#include "WStructs/WTextureStructs.h"
#include <memory>


WCLASS()
class WCORE_API WTextureAsset : public WAsset
{
    WOBJECT_BODY(WTextureAsset)

    public:
        void SetTexture(const WTextureStruct& texture);
        void SetTexture(WTextureStruct&& texture);

        const WTextureStruct& GetTexture() const;

    private:
        WTextureStruct texture_{};
};

