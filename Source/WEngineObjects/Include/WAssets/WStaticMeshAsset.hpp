#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WAsset.hpp"
#include "WStructs/WGeometryStructs.hpp"

#include "WStaticMeshAsset.WEngine.hpp"

WCLASS()
class WENGINEOBJECTS_API WStaticMeshAsset : public WAsset
{
    WOBJECT_BODY(WStaticMeshAsset)

public:

    // DEPRECATED
    constexpr void SetMesh(const WMeshStruct& in_mesh) noexcept {
        mesh_ = in_mesh;
    }

    // DEPRECATED
    constexpr void SetMesh(WMeshStruct && in_mesh) noexcept {
        mesh_ = std::move(in_mesh);
    }

    // DEPRECATED
    WNODISCARD constexpr const WMeshStruct& GetMesh() const noexcept {
        return mesh_;
    }

    constexpr void SetMesh(const WMeshStruct& in_mesh, const WAssIdxId & in_id);

    constexpr const WMeshStruct & GetMesh(const WAssIdxId & in_index) const noexcept;

private:
    
    // DEPRECATED
    WMeshStruct mesh_{};
    
    WMeshsesStruct meshes_{};
    
};

