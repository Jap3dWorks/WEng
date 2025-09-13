#include "WVulkan/WVkAssetResources.hpp"
#include "WCore/WCore.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVulkan.hpp"
#include <vector>

WVkAssetResources::WVkAssetResources()=default;

WVkAssetResources::~WVkAssetResources()=default;

WVkAssetResources::WVkAssetResources(
    const WVkDeviceInfo & in_device_info,
    const WVkCommandPoolInfo & in_command_pool_info
    )  :
    device_info_(in_device_info),
    command_pool_info_(in_command_pool_info),
    texture_collection_(),
    static_mesh_collection_()
{
    InitializeStaticMeshFunctions();
    InitializeTextureFunctions();
}

WVkAssetResources::WVkAssetResources(WVkAssetResources && other) :
    device_info_(std::move(other.device_info_)),
    command_pool_info_(std::move(other.command_pool_info_)),
    texture_collection_(std::move(other.texture_collection_)),
    static_mesh_collection_(std::move(other.static_mesh_collection_))
{
    InitializeStaticMeshFunctions();
    InitializeTextureFunctions();

    other.device_info_ = {};
    other.command_pool_info_ = {};
}

WVkAssetResources & WVkAssetResources::operator=(WVkAssetResources && other) {
    if (this != &other) {
        device_info_ = std::move(other.device_info_);
        command_pool_info_ = std::move(other.command_pool_info_);
        texture_collection_ = std::move(other.texture_collection_);
        static_mesh_collection_ = std::move(other.static_mesh_collection_);
        
        InitializeStaticMeshFunctions();
        InitializeTextureFunctions();

        other.device_info_ = {};
        other.command_pool_info_ = {};
    }

    return *this;
}

void WVkAssetResources::UnloadTexture(const WAssetId & in_id) {
    texture_collection_.UnloadResource(in_id);
}

void WVkAssetResources::UnloadStaticMesh(const WAssetIndexId & in_id) {
    static_mesh_collection_.UnloadResource(in_id);
}

const WVkTextureInfo & WVkAssetResources::TextureInfo(const WAssetId & in_id) const {
    return texture_collection_.GetData(in_id);
}

const WVkMeshInfo & WVkAssetResources::StaticMeshInfo(const WAssetIndexId & in_id) const {
    return static_mesh_collection_.GetData(in_id);
}

void WVkAssetResources::Clear() {
    texture_collection_.Clear();
    static_mesh_collection_.Clear();    
}

void WVkAssetResources::InitializeTextureFunctions() {

    texture_collection_.SetClearFn(
        [this](WVkTextureInfo & in_texture_info) -> void {
            WVulkan::Destroy(
                in_texture_info,
                device_info_
                );
        }
        );
}

void WVkAssetResources::InitializeStaticMeshFunctions() {
    
    static_mesh_collection_.SetClearFn(
        [this] (WVkMeshInfo & in_mesh_info) -> void {
            WVulkan::Destroy(
                in_mesh_info,
                device_info_
                );
        });    
}
