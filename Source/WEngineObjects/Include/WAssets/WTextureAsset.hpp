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

        void SetTexture(const wtp::texture::WTexture& texture);

        void SetTexture(wtp::texture::WTexture&& texture);

        const wtp::texture::WTexture& GetTexture() const;

    private:
        wtp::texture::WTexture texture_{};
};

