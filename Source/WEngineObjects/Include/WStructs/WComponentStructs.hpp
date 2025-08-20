#pragma once

#include "WCore/WCore.hpp"
#include <glm/glm.hpp>

enum class ERotationOrder {
    xyz,
    xzy,
    yxz,
    yzx,
    zxy,
    zyx
};

struct WTransformStruct
{
public:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    ERotationOrder rotation_order{ERotationOrder::zxy};
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
    WAssetId render_pipeline_asset{};
    WAssetId render_pipeline_parameters_asset{};
};
