#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WAsset.hpp"
#include "WStructs/WTextureStructs.hpp"
#include <memory>

#include "WTextureAsset.WEngine.hpp"

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

