#pragma once

#include "WEngine/WEngine.hpp"

namespace weng::defaults {

    WEngine DefaultEngine();

    inline constexpr
    std::string_view const NULL_RGBA_TEXTURE_ASSET_PATH{
        "/Content/Assets/Textures/null_rgba_texture:null_rgba_texture"
    };

    inline constexpr
    std::string_view const NULL_TEXTURE_ASSET_PATH {
        "/Content/Assets/Textures/null_texture:null_texture"
    };

    inline constexpr
    std::string_view const NULL_NORMAL_TEXTURE_ASSET_PATH{
        "/Content/Assets/Textures/null_normal_texture:null_normal_texture"
    };

    // TODO default geometries.

    inline constexpr
    std::string_view const PBR_PIPELINE_SHADER_PATH{
        "/Content/Shaders/WRender_PBR.gbuffer.spv"
    };

    inline constexpr
    std::string_view const PBR_PIPELINE_ASSET_PATH{
        "/Content/Assets/Pipelines/pbr_pipeline:pbr_pipeline"
    };

    inline constexpr
    std::string_view const PBR_PIPE_PARAMS_NULL_ASSET_PATH {
        "/Content/Assets/Pipelines/pbr_pparams_null:pbr_pparams_null"
    };

    inline constexpr
    std::string_view const CAMERA_MAPPING_ASSET_PATH{
        "/Content/Input/CameraMapping:CameraMapping"
    };

    inline constexpr
    std::string_view const FRONT_ACTION_ASSET_PATH{
        "/Content/Input/CameraMapping:CameraMapping"
    };

    inline constexpr
    std::string_view const BACK_ACTION_ASSET_PATH{
        "/Content/Input/BackAction:BackAction"        
    };

    inline constexpr
    std::string_view const LEFT_ACTION_ASSET_PATH{
        "/Content/Input/LeftAction:LeftAction"        
    };

    inline constexpr
    std::string_view const RIGHT_ACTION_ASSET_PATH{
        "/Content/Input/RightAction:RightAction"        
    };

    inline constexpr
    std::string_view const MOUSE_MOVEMENT_ACTION_ASSET_PATH{
        "/Content/Input/MouseMovement:MouseMovement"        
    };

}
