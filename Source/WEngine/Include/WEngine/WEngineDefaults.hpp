#pragma once

#include "WEngine/WEngine.hpp"

namespace weng::defaults {

    WEngine DefaultEngine();

    inline constexpr
    std::string_view const NULL_RGBA_TEXTURE_ASSET_PATH {
        "/Content/Assets/Textures/null_texture:null_texture"
    };

    inline constexpr
    std::string_view const NULL_NORMAL_TEXTURE_ASSET_PATH{
        "/Content/Assets/Textures/null_normal_texture:null_normal_texture"
    };

    // TODO default geometries.
    inline constexpr
    std::string_view const PBR_PIPELINE_SHADER_PATH{
        "/Content/Shaders/WRender_GBuffer.gbuffer.spv"
    };

    inline constexpr
    std::string_view const PBR_PIPELINE_ASSET_PATH{
        "/Content/Assets/Pipelines/pbr_pipeline:pbr_pipeline"
    };

}
