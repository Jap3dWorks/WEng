#pragma once

#include "WCore/TObjectDataBase.hpp"
#include "WCore/WCore.hpp"

#include "WStructs/WGeometryStructs.hpp"
#include "WStructs/WTextureStructs.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVulkan.hpp"

#include "WVulkan/WVulkanStructs.hpp"

/**
 * @brief Manage the lifetime of asset render resources like geometries or textures.
 */
class WRENDER_API WVkAssetResources {
private:

using WVkTextureDb = TObjectDataBase<WVkTextureInfo, void, WAssetId>;

using WVkAssetDb = TObjectDataBase<WVkMeshInfo, void, WAssetIndexId>;

public:

    WVkAssetResources();

    WVkAssetResources(
        const WVkDeviceInfo & in_device_info,
        const WVkCommandPoolInfo & in_command_pool_info
        );

    virtual ~WVkAssetResources();

    WVkAssetResources(const WVkAssetResources & other) = delete;

    WVkAssetResources(WVkAssetResources && other);

    WVkAssetResources & operator=(const WVkAssetResources & other) = delete;

    WVkAssetResources & operator=(WVkAssetResources && other);

    // Texture

    void LoadTexture(const WAssetId & in_id, const WTextureStruct & in_texture) {
        texture_collection_.CreateAt(
            in_id,
            [this, &in_texture](const WAssetId & _id) -> WVkTextureInfo {
                WVkTextureInfo result;
                WVulkan::CreateTexture(
                    result,
                    in_texture,
                    device_info_,
                    command_pool_info_
                    );
                return result;
            });
    }

    void UnloadTexture(const WAssetId & in_id);

    const WVkTextureInfo & TextureInfo(const WAssetId & in_id) const;

    // Static Mesh

    void LoadStaticMesh(const WAssetIndexId & in_id, const WMeshStruct & in_mesh) {

        static_mesh_collection_.CreateAt(
            in_id,
            [this, &in_mesh] (const WAssetIndexId & in_id) -> WVkMeshInfo {
                WVkMeshInfo result;
                WVulkan::CreateMeshBuffers(
                    result,
                    in_mesh.vertices.data(),
                    sizeof(decltype(in_mesh.vertices)::value_type) * in_mesh.vertices.size(),
                    in_mesh.indices.data(),
                    sizeof(decltype(in_mesh.indices)::value_type) * in_mesh.indices.size(),
                    in_mesh.indices.size(),
                    device_info_,
                    command_pool_info_
                    );
                
                return result;
            }
            );
    }

    void UnloadStaticMesh(const WAssetIndexId & in_id);

    const WVkMeshInfo & StaticMeshInfo(const WAssetIndexId & in_id) const;

    void Clear();

private:
    
    WVkTextureDb texture_collection_;
             
    WVkAssetDb static_mesh_collection_;

    std::unordered_map<WAssetId, std::vector<WSubIdxId>> mesh_indexes_;

    WVkDeviceInfo device_info_;
    WVkCommandPoolInfo command_pool_info_;
    
};

