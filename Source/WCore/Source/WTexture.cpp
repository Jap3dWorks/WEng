#include "WAssets/WTextureAsset.h"


// WTextureAsset
// -------------
void WTextureAsset::SetTexture(const WTexture& texture)
{
    texture_ = texture;
}

const WTexture& WTextureAsset::GetTexture() const
{
    return texture_;
}
