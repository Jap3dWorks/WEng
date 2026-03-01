#include "WVulkan/WVkAssetResources.hpp"
#include "WCore/WCore.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVulkan.hpp"

WVkAssetResources::WVkAssetResources()=default;

WVkAssetResources::~WVkAssetResources() {
    Destroy();
}

WVkAssetResources::WVkAssetResources(
    const VkDevice & in_device,
    const VkPhysicalDevice & in_physical_device,
    const VkQueue & in_graphics_queue,
    const VkCommandPool & in_command_pool
    )  :
    device_(in_device),
    physical_device_(in_physical_device),
    graphics_queue_(in_graphics_queue),
    command_pool_(in_command_pool),
    texture_collection_(),
    static_mesh_collection_()
{}

WVkAssetResources::WVkAssetResources(WVkAssetResources && other) :
    device_(std::move(other.device_)),
    physical_device_(std::move(other.physical_device_)),
    graphics_queue_(std::move(other.graphics_queue_)),
    command_pool_(std::move(other.command_pool_)),
    texture_collection_(std::move(other.texture_collection_)),
    static_mesh_collection_(std::move(other.static_mesh_collection_))
{
    other.device_ = VK_NULL_HANDLE;
    other.physical_device_ = VK_NULL_HANDLE;
    other.graphics_queue_ = VK_NULL_HANDLE;
    other.command_pool_ = VK_NULL_HANDLE;
}

WVkAssetResources & WVkAssetResources::operator=(WVkAssetResources && other) {
    if (this != &other) {
        Destroy();

        device_ = std::move(other.device_);
        physical_device_ = std::move(other.physical_device_);
        graphics_queue_ = std::move(other.graphics_queue_);
        command_pool_ = std::move(other.command_pool_);
        texture_collection_ = std::move(other.texture_collection_);
        static_mesh_collection_ = std::move(other.static_mesh_collection_);
        
        other.device_ = VK_NULL_HANDLE;
        other.physical_device_ = VK_NULL_HANDLE;
        other.graphics_queue_ = VK_NULL_HANDLE;
        other.command_pool_ = VK_NULL_HANDLE;
    }

    return *this;
}

void WVkAssetResources::UnloadTexture(const WAssetId & in_id) {
    texture_collection_.Remove(
        in_id,
        [this](WVkTextureInfo & in_texture_info) -> void {
            WVulkan::Destroy(
                in_texture_info,
                device_
                );
        }
        );
}

void WVkAssetResources::UnloadStaticMesh(const WAssetIndexId & in_id) {

    static_mesh_collection_.Remove(
        in_id,
        [this] (WVkMeshInfo & in_mesh_info) -> void {
            WVulkan::Destroy(
                in_mesh_info,
                device_
                );
        });    
}

const WVkTextureInfo & WVkAssetResources::TextureInfo(const WAssetId & in_id) const {
    return texture_collection_.Get(in_id);
}

const WVkMeshInfo & WVkAssetResources::StaticMeshInfo(const WAssetIndexId & in_id) const {
    return static_mesh_collection_.Get(in_id);
}

void WVkAssetResources::Clear() {
    if (device_ != VK_NULL_HANDLE) {
        texture_collection_.Clear(
            [this](WVkTextureInfo & in_texture_info) -> void {
                WVulkan::Destroy(
                    in_texture_info,
                    device_
                    );
            }
            );

        static_mesh_collection_.Clear(
            [this] (WVkMeshInfo & in_mesh_info) -> void {
                WVulkan::Destroy(
                    in_mesh_info,
                    device_
                    );
            });            
    }
}

void WVkAssetResources::Destroy() {
    if (device_ != VK_NULL_HANDLE) {
        Clear();

        device_ = VK_NULL_HANDLE;
        physical_device_ = VK_NULL_HANDLE;
        graphics_queue_ = VK_NULL_HANDLE;
        command_pool_ = VK_NULL_HANDLE;
    }
}
