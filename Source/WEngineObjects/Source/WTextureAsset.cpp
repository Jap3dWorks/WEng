#include "WAssets/WTextureAsset.hpp"


// WTextureAsset
// -------------
void WTextureAsset::SetTexture(const wtp::texture::WTexture& texture)
{
    texture_ = texture;
}

const wtp::texture::WTexture& WTextureAsset::GetTexture() const
{
    return texture_;
}

void WTextureAsset::SetTexture(wtp::texture::WTexture&& texture)
{
    texture_ = std::move(texture);
}
