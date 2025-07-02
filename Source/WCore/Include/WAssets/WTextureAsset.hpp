#pragma once

#include "WCore/WCore.hpp"
#include "WAssets/WAsset.hpp"
#include "WStructs/WTextureStructs.hpp"
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

