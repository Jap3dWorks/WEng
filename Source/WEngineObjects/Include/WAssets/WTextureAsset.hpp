#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WAsset.hpp"
#include "WCoreTypes/WTexture.hpp"
#include <memory>

#include "WTextureAsset.WEngine.hpp"

WCLASS()
class WCORE_API WTextureAsset : public WAsset
{
    WOBJECT_BODY(WTextureAsset)

    public:

        void SetTexture(const wct::texture::WTexture& texture);

        void SetTexture(wct::texture::WTexture&& texture);

        const wct::texture::WTexture& GetTexture() const;

    private:
        wct::texture::WTexture texture_{};
};

