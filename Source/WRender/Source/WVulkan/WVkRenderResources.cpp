#include "WVulkan/WVkRenderResources.hpp"
#include "WVulkan/WVkRenderCore.hpp"
#include "WVulkan/WVulkan.hpp"
#include <vector>

WVkRenderResources::WVkRenderResources() :
    texture_collection_(),
    static_mesh_collection_()
{}

WVkRenderResources::~WVkRenderResources() {
    Clear();
}

WVkRenderResources::WVkRenderResources(
    const WVkDeviceInfo & in_device_info,
    const WVkCommandPoolInfo & in_command_pool_info
    )  :
    texture_collection_(
        [this,
         in_device_info,
         in_command_pool_info](const WId & in_id) -> WVkTextureInfo {
            WVkTextureInfo result;
            WVulkan::Create(
                result,
                texture_collection_.GetAsset(in_id).GetTexture(),
                in_device_info,
                in_command_pool_info
                );
            
            return result;
        },
        [this,
         in_device_info](WVkTextureInfo & in_info) -> void {
            WVulkan::Destroy(
                in_info,
                in_device_info
                );
        }
        ),
    static_mesh_collection_(
        [this,
         in_device_info,
         in_command_pool_info] (const WId & in_id) -> WVkMeshInfo {
            WVkMeshInfo result;
            WVulkan::Create(
                result,
                static_mesh_collection_.GetAsset(in_id).GetMesh(),
                in_device_info,
                in_command_pool_info
                );
            return result;
        },
        [this,
         in_device_info] (WVkMeshInfo & in_mesh_info) -> void {
            WVulkan::Destroy(
                in_mesh_info,
                in_device_info
                );
        }
        ) {}

WVkRenderResources::WVkRenderResources(WVkRenderResources && other) :
    texture_collection_(std::move(other.texture_collection_)),
    static_mesh_collection_(std::move(other.static_mesh_collection_))
{}

WVkRenderResources & WVkRenderResources::operator=(WVkRenderResources && other) {
    if (this != &other) {
        texture_collection_ = std::move(other.texture_collection_);
        static_mesh_collection_ = std::move(other.static_mesh_collection_);
    }

    return *this;
}

void WVkRenderResources::RegisterTexture(WTextureAsset & in_texture_asset) {
    return texture_collection_.RegisterAsset(in_texture_asset);
}

void WVkRenderResources::UnregisterTexture(const  WId & in_id) {
    texture_collection_.UnregisterAsset(in_id);
}

void WVkRenderResources::LoadTexture(const WId & in_id) {
    texture_collection_.LoadAsset(in_id);
}

void WVkRenderResources::UnloadTexture(const WId & in_id) {
    texture_collection_.UnloadAsset(in_id);
}

void WVkRenderResources::RegisterStaticMesh(WStaticMeshAsset & in_static_mesh_asset) {
    return static_mesh_collection_.RegisterAsset(in_static_mesh_asset);
}

void WVkRenderResources::UnregisterStaticMesh(const WId & in_id) {
    static_mesh_collection_.UnregisterAsset(in_id);
}

void WVkRenderResources::LoadStaticMesh(const WId & in_id) {
    static_mesh_collection_.LoadAsset(in_id);
}

void WVkRenderResources::UnloadStaticMesh(const WId & in_id) {
    static_mesh_collection_.UnloadAsset(in_id);
}

const WVkTextureInfo & WVkRenderResources::TextureInfo(const WId & in_id) const {
    return texture_collection_.GetData(in_id);
}

const WVkMeshInfo & WVkRenderResources::StaticMeshInfo(const WId & in_id) const {
    return static_mesh_collection_.GetData(in_id);
}

void WVkRenderResources::Clear() {
    texture_collection_.Clear();
    static_mesh_collection_.Clear();    
}

