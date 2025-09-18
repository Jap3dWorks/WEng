#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WCoreMacros.hpp"
#include "WCore/WConcepts.hpp"
#include "WEngineObjects/WAsset.hpp"
#include "WStructs/WGeometryStructs.hpp"
#include "WCore/TIterator.hpp"

#include <array>

#include "WStaticMeshAsset.WEngine.hpp"

WCLASS()
class WENGINEOBJECTS_API WStaticMeshAsset : public WAsset
{
    WOBJECT_BODY(WStaticMeshAsset)

    using WMeshArray = std::array<WMeshStruct, WENG_MAX_ASSET_IDS>;

public:

    constexpr void SetMesh(const WMeshStruct & in_mesh, const WSubIdxId & in_id=0) {
        meshes_[in_id.GetId()] = in_mesh;
    }

    constexpr void SetMesh(WMeshStruct && in_mesh, const WSubIdxId & in_id=0) noexcept {
        meshes_[in_id.GetId()] = std::move(in_mesh);
    }

    constexpr const WMeshStruct & GetMesh(const WSubIdxId & in_index=0) const noexcept {
        return meshes_[in_index.GetId()];
    }

    std::uint8_t MeshCount() const {
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

    template<CCallable<void, WStaticMeshAsset *, const WSubIdxId &, WMeshStruct&> F>
    void ForEachMesh(F && in_fn) {
        for(std::uint32_t i=0; i<meshes_.size(); i++) {
            WMeshStruct & m = meshes_[i];
            if(m.indices.empty()) {
                break;
            }

            in_fn(this, i, m);
        };
    }

private:

    std::array<WMeshStruct, WENG_MAX_ASSET_IDS> meshes_{};
    
};

