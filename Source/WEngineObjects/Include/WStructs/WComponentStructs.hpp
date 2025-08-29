#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WCoreMacros.hpp"
#include "WStructs/WMathStructs.hpp"
#include <array>

#include <glm/glm.hpp>

struct WTransformStruct
{
public:
    glm::vec3 position{0.0, 0.0, 0.0};
    glm::vec3 rotation{0.0, 0.0, 0.0};
    glm::vec3 scale{1.0, 1.0, 1.0};
    ERotationOrder rotation_order{ERotationOrder::zxy};

    glm::mat4 transform_matrix{1.0};
};

struct WCameraStruct
{
    glm::vec3 point{0};
    float angle_of_view{45.f};
    float near_clipping{0.01f};
    float far_clipping{100.f};
};

struct WStaticMeshStruct
{
    WAssetId static_mesh_asset{};
    std::array<WAssetId, WENG_MAX_ASSET_IDS> render_pipeline_assets{};
    // WAssetId render_pipeline_asset{};
    std::array<WAssetId, WENG_MAX_ASSET_IDS> render_pipeline_params_assets{};
    // WAssetId render_pipeline_parameters_asset{};
};
