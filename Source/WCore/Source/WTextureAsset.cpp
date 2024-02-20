#include "WAssets/WTextureAsset.h"


// WTextureAsset
// -------------
void WTextureAsset::SetTexture(const WTextureStruct& texture)
{
    texture_ = texture;
}

const WTextureStruct& WTextureAsset::GetTexture() const
{
    return texture_;
}

void WTextureAsset::SetTexture(WTextureStruct&& texture)
{
    texture_ = std::move(texture);
}
