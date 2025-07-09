#pragma once

#include "WCore/WCore.hpp"

#include <vector>

class IRender {
public:

    virtual void Draw()=0;

    virtual void AddPipelineBinding(
        WId pipeline_id,
        WId descriptor_set_id,
        WId in_mesh_id,
        const std::vector<WId> & in_textures,
        const std::vector<uint32_t> & in_textures_bindings
        )=0;

    virtual void Clear()=0;

};
