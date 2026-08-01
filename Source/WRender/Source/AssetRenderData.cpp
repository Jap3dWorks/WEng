#include "WVulkan/RAII/AssetRenderData.hpp"
#include "WCore/TVisitor.hpp"
#include "WCore/WCore.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/Vk/WVulkan.hpp"
#include "WVulkan/Vk/WVkBuffer.hpp"

#include <algorithm>

wvk::raii::AssetRenderData::~AssetRenderData() {
    Destroy();
}

wvk::raii::AssetRenderData::AssetRenderData(
    const VkDevice & device,
    const VkPhysicalDevice & physical_device,
    const VkQueue & graphics_queue,
    const VkCommandPool & command_pool
    )  :
    vkn_(device, physical_device, graphics_queue, command_pool),
    texture_collection_(),
    static_mesh_collection_()
{}

wvk::raii::AssetRenderData::AssetRenderData(wvk::raii::AssetRenderData && other) :
    vkn_(std::move(other.vkn_)),
    texture_collection_(std::move(other.texture_collection_)),
    static_mesh_collection_(std::move(other.static_mesh_collection_))
{
    other.vkn_ = {};
}

wvk::raii::AssetRenderData & wvk::raii::AssetRenderData::operator=(wvk::raii::AssetRenderData && other) {
    if (this != &other) {
        Destroy();
        
        vkn_ = std::move(other.vkn_);
        texture_collection_ = std::move(other.texture_collection_);
        static_mesh_collection_ = std::move(other.static_mesh_collection_);
        
        other.vkn_ = {};
    }

    return *this;
}

void wvk::raii::AssetRenderData::UnloadTexture(const wcr::wid::WAssetId & in_id) {
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

void wvk::raii::AssetRenderData::UnloadStaticMesh(const wcr::wid::WTypeAssetIndexId & in_id) {

    static_mesh_collection_.Remove(
        in_id,
        [this] (WVkMesh & in_mesh_info) -> void {
            wvk::mesh::Destroy(
                in_mesh_info,
                vkn_.device
                );
        });    
}

const WVkMesh & wvk::raii::AssetRenderData::StaticMeshInfo(const wcr::wid::WTypeAssetIndexId & in_id) const {
    return static_mesh_collection_.Get(in_id);
}

void wvk::raii::AssetRenderData::Clear() {
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
            [this] (WVkMesh & in_mesh_info) -> void {
                wvk::mesh::Destroy(
                    in_mesh_info,
                    vkn_.device
                    );
            });
    }
}

void wvk::raii::AssetRenderData::Destroy() {
    if (vkn_.device != VK_NULL_HANDLE) {
        Clear();
        vkn_.device = VK_NULL_HANDLE;
        vkn_.physical_device = VK_NULL_HANDLE;
        vkn_.graphics_queue = VK_NULL_HANDLE;
        vkn_.command_pool = VK_NULL_HANDLE;
    }
}


