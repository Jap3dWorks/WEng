#include "WVulkan/WVkRAII/WVkAssetRenderDataRAII.hpp"
#include "WCore/WCore.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVk/WVulkan.hpp"

WVkAssetRenderDataRAII::~WVkAssetRenderDataRAII() {
    Destroy();
}

WVkAssetRenderDataRAII::WVkAssetRenderDataRAII(
    const VkDevice & device,
    const VkPhysicalDevice & physical_device,
    const VkQueue & graphics_queue,
    const VkCommandPool & command_pool
    )  :
    vkn_(device, physical_device, graphics_queue, command_pool),
    texture_collection_(),
    static_mesh_collection_()
{}

WVkAssetRenderDataRAII::WVkAssetRenderDataRAII(WVkAssetRenderDataRAII && other) :
    vkn_(std::move(other.vkn_)),
    texture_collection_(std::move(other.texture_collection_)),
    static_mesh_collection_(std::move(other.static_mesh_collection_))
{
    other.vkn_ = {};
}

WVkAssetRenderDataRAII & WVkAssetRenderDataRAII::operator=(WVkAssetRenderDataRAII && other) {
    if (this != &other) {
        Destroy();
        vkn_ = std::move(other.vkn_);
        texture_collection_ = std::move(other.texture_collection_);
        static_mesh_collection_ = std::move(other.static_mesh_collection_);
        
        other.vkn_ = {};
    }

    return *this;
}

void WVkAssetRenderDataRAII::UnloadTexture(const wcr::wid::WAssetId & in_id) {
    texture_collection_.Remove(
        in_id.GetId(),
        [this](WVkTextureInfo & in_texture_info) -> void {
            wvk::texture::DestroyTexture(
                in_texture_info,
                vkn_.device
                );
        }
        );
}

void WVkAssetRenderDataRAII::UnloadStaticMesh(const wcr::wid::WTypeAssetIndexId & in_id) {

    static_mesh_collection_.Remove(
        in_id,
        [this] (WVkMeshInfo & in_mesh_info) -> void {
            wvk::mesh::Destroy(
                in_mesh_info,
                vkn_.device
                );
        });    
}

const WVkTextureInfo & WVkAssetRenderDataRAII::TextureInfo(const wcr::wid::WAssetId & in_id) const {
    return texture_collection_.Get(in_id.GetId());
}

const WVkMeshInfo & WVkAssetRenderDataRAII::StaticMeshInfo(const wcr::wid::WTypeAssetIndexId & in_id) const {
    return static_mesh_collection_.Get(in_id);
}

void WVkAssetRenderDataRAII::Clear() {
    if (vkn_.device != VK_NULL_HANDLE) {
        texture_collection_.Clear(
            [this](WVkTextureInfo & in_texture_info) -> void {
                wvk::texture::DestroyTexture(
                    in_texture_info,
                    vkn_.device
                    );
            }
            );

        static_mesh_collection_.Clear(
            [this] (WVkMeshInfo & in_mesh_info) -> void {
                wvk::mesh::Destroy(
                    in_mesh_info,
                    vkn_.device
                    );
            });            
    }
}

void WVkAssetRenderDataRAII::Destroy() {
    if (vkn_.device != VK_NULL_HANDLE) {
        Clear();

        vkn_.device = VK_NULL_HANDLE;
        vkn_.physical_device = VK_NULL_HANDLE;
        vkn_.graphics_queue = VK_NULL_HANDLE;
        vkn_.command_pool = VK_NULL_HANDLE;
    }
}
