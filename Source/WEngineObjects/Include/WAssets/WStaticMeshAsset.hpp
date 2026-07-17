#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WConcepts.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WEngineObjects/WAsset.hpp"
#include "WCoreTypes/WGeometry.hpp"
#include "WCore/TIterator.hpp"

#include <array>

#include "WStaticMeshAsset.WEngine.hpp"

class WENGINEOBJECTS_API WStaticMeshAsset : public WAsset
{
    WOBJECT_BODY;

public:

    static inline constexpr std::uint8_t MAX_MESH_COUNT{8};

    using MeshList =
        std::array<wct::geometry::WMesh, MAX_MESH_COUNT>;

    using PipelineAssignments =
        wct::render::RPipeAssignments<MAX_MESH_COUNT>;

public:

    WPROPERTY(MeshList, meshes,);
    WPROPERTY(PipelineAssignments, pipeline_assignments,);

public:
    
    constexpr void SetMesh(wct::geometry::WMesh const & in_mesh, wcr::wid::WSubIdxId const & in_id=0) {
        meshes[in_id.GetId()] = in_mesh;
    }

    constexpr void SetMesh(wct::geometry::WMesh && in_mesh, wcr::wid::WSubIdxId const & in_id=0) noexcept {
        meshes[in_id.GetId()] = std::move(in_mesh);
    }

    constexpr wct::geometry::WMesh const & GetMesh(wcr::wid::WSubIdxId const & in_index=0) const noexcept {
        return meshes[in_index.GetId()];
    }

    std::uint8_t MeshCount() const {
        std::uint8_t r = 0;
        for(auto & v : meshes) {
            if(!v.indices.empty()) {
                r++;
            }
            else {
                return r;
            }
        }
        return r;
    }

    template<CCallable<void, WStaticMeshAsset *, wcr::wid::WSubIdxId, wct::geometry::WMesh&> F>
    void ForEachMesh(F && in_fn) {
        for(std::uint32_t i=0; i<meshes.size(); i++) {
            wct::geometry::WMesh & m = meshes[i];
            if(m.indices.empty()) {
                break;
            }
            in_fn(this, i, m);
        };
    }

    constexpr void SetPipelineAssignment(
        wct::render::RPipeAssignment in_assignment,
        wcr::wid::WSubIdxId in_index
        ) {
        pipeline_assignments[in_index.GetId()] =
            std::move(in_assignment);
    }

    constexpr wct::render::RPipeAssignment GetPipelineAssignment(
        wcr::wid::WSubIdxId in_index
        ) const {
        return pipeline_assignments[in_index.GetId()];
    }
};

