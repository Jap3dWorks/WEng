#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WCoreMacros.hpp"
#include "WCoreTypes/WMathStructs.hpp"
#include <array>

#include <glm/glm.hpp>

// TODO move each struct to its logical object.


// DEPRECATED
struct WTransformStruct
{
public:
    glm::vec3 position{0.0, 0.0, 0.0};
    glm::vec3 rotation{0.0, 0.0, 0.0};
    glm::vec3 scale{1.0, 1.0, 1.0};
    ERotationOrder rotation_order{ERotationOrder::zxy};

    glm::mat4 transform_matrix{1.0};
};

namespace wcm::types {
    struct WPipelineAssignment {
        WAssetId pipeline{};
        WAssetId params{};
    };

    template<std::uint8_t Max=WENG_MAX_ASSET_IDS>
    using WPipelineAssignments = std::array<WPipelineAssignment, Max>;
}
