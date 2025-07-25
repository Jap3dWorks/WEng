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

    constexpr void SetMesh(const WMeshStruct& in_mesh) noexcept {
        mesh_ = in_mesh;
    }

    constexpr void SetMesh(WMeshStruct && in_mesh) noexcept {
        mesh_ = std::move(in_mesh);
    }

    WNODISCARD constexpr const WMeshStruct& GetMesh() const noexcept {
        return mesh_;
    }

private:

    WMeshStruct mesh_{};

};

