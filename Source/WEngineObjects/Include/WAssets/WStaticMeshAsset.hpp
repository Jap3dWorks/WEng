#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WCoreMacros.hpp"
#include "WEngineObjects/WAsset.hpp"
#include "WStructs/WGeometryStructs.hpp"
#include <array>

#include "WStaticMeshAsset.WEngine.hpp"

WCLASS()
class WENGINEOBJECTS_API WStaticMeshAsset : public WAsset
{
    WOBJECT_BODY(WStaticMeshAsset)

public:

    constexpr void SetMesh(const WMeshStruct & in_mesh, const WAssIdxId & in_id=0) {
        meshes_[in_id.GetId()] = in_mesh;
    }

    constexpr void SetMesh(WMeshStruct && in_mesh, const WAssIdxId & in_id=0) noexcept {
        meshes_[in_id.GetId()] = std::move(in_mesh);
    }

    constexpr const WMeshStruct & GetMesh(const WAssIdxId & in_index=0) const noexcept {
        return meshes_[in_index.GetId()];
    }

    std::uint8_t MeshesCount() const {
        std::uint8_t r = 0;
        for(auto & v : meshes_) {
            if(!v.indices.empty()) {
                r++;
            }
            else {
                return r;
            }
        }
        return r;
    }

private:
    
    std::array<WMeshStruct, WENG_MAX_ASSET_IDS> meshes_{};
    
};

