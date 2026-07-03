#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WCoreMacros.hpp"
#include "WCore/WConcepts.hpp"
#include "WEngineObjects/WAsset.hpp"
#include "WCoreTypes/WGeometry.hpp"
#include "WCore/TIterator.hpp"

#include <array>

#include "WStaticMeshAsset.WEngine.hpp"

using WMeshList = std::array<wct::geometry::WMesh, WENG_MAX_ASSET_IDS>;

class WENGINEOBJECTS_API WStaticMeshAsset : public WAsset
{
    WOBJECT_BODY;

public:

    WPROPERTY(WMeshList, mesh_list, );

public:

    constexpr void SetMesh(const wct::geometry::WMesh & in_mesh, const WSubIdxId & in_id=0) {
        mesh_list[in_id.GetId()] = in_mesh;
    }

    constexpr void SetMesh(wct::geometry::WMesh && in_mesh, const WSubIdxId & in_id=0) noexcept {
        mesh_list[in_id.GetId()] = std::move(in_mesh);
    }

    constexpr const wct::geometry::WMesh & GetMesh(const WSubIdxId & in_index=0) const noexcept {
        return mesh_list[in_index.GetId()];
    }

    std::uint8_t MeshCount() const {
        std::uint8_t r = 0;
        for(auto & v : mesh_list) {
            if(!v.indices.empty()) {
                r++;
            }
            else {
                return r;
            }
        }
        return r;
    }

    template<CCallable<void, WStaticMeshAsset *, const WSubIdxId &, wct::geometry::WMesh&> F>
    void ForEachMesh(F && in_fn) {
        for(std::uint32_t i=0; i<mesh_list.size(); i++) {
            wct::geometry::WMesh & m = mesh_list[i];
            if(m.indices.empty()) {
                break;
            }

            in_fn(this, i, m);
        };
    }
};

