#pragma once

#include "WCore/WCore.hpp"
#include <glm/glm.hpp>

struct WTransformStruct
{
public:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

struct WCameraStruct
{
    glm::vec3 point;
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
