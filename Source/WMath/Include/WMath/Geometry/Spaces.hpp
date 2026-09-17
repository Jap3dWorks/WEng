#pragma once

#include <glm/glm.hpp>
#include <array>

namespace wcl::spaces {

    /**
     * Returns {u,v} as v0 = u*v1 + v*v2.
     * To ensure a correct behaviour v0, v1 and v2 are required to be part of the same plane.
     */
    inline constexpr std::array<float, 2> AsVectorSum(
        glm::vec3 v0,
        glm::vec3 v1,
        glm::vec3 v2
        ) {

        float v = (v1.x * v0.y - v1.y * v0.x) / (v2.y * v1.x - v1.y * v2.x);
        float u = (v0.x - v * v2.x) / v1.x;

        return {u,v};
    }

}
