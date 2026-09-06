#pragma once

#include "WColliders/Shapes.hpp"
#include "glm/ext/matrix_float4x4.hpp"

#include <optional>

#include <glm/glm.hpp>
#include <glm/matrix.hpp>

namespace wcl::collision {

    struct ColData{};

    inline std::optional<ColData> Intersects(
        wcl::shapes::Cube, wcl::shapes::Cube, glm::mat4
        ) {
        return std::nullopt;
    }

    inline std::optional<ColData> Intersects(
        wcl::shapes::Cube, wcl::shapes::Sphere, glm::mat4
        ) {
        return std::nullopt;
    }

    inline std::optional<ColData> Intersects(
        wcl::shapes::Cube, wcl::shapes::Capsule, glm::mat4
        ) {
        return std::nullopt;
    }

    inline std::optional<ColData> Intersects(
        wcl::shapes::Sphere, wcl::shapes::Sphere, glm::mat4
        ) {
        return std::nullopt;
    }
    
    inline std::optional<ColData> Intersects(
        wcl::shapes::Sphere, wcl::shapes::Capsule, glm::mat4
        ) {
        return std::nullopt;
    }

    inline std::optional<ColData> Intersects(
        wcl::shapes::Capsule, wcl::shapes::Capsule, glm::mat4
        ) {
        return std::nullopt;
    }

}
