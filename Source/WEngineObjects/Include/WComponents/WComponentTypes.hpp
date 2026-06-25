#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WCoreMacros.hpp"
#include <array>

#include <glm/glm.hpp>

namespace wcm::types {
    struct WPipelineAssignment {
        WAssetId pipeline{};
        WAssetId params{};
    };

    template<std::uint8_t Max=WENG_MAX_ASSET_IDS>
    using WPipelineAssignments = std::array<WPipelineAssignment, Max>;
}
