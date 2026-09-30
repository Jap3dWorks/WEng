#pragma once

#include "WMath/Geometry/Shape.hpp"
#include <limits>
#include <numeric>

namespace wmath::geometry::shape {

    inline constexpr AABB ToAABB(Box const & box) {
        return {
            .min{-box.x, -box.y, -box.z},
            .max{box.x, box.y, box.z}
        };
    }

    inline constexpr AABB ToAABB(Box const & box, glm::mat4 const & transform) {
        auto verts = Box::Vertices(box, transform);

        glm::vec3 max{std::numeric_limits<float>::min()};
        glm::vec3 min{std::numeric_limits<float>::max()};

        for(auto & v : verts) {
            for (std::uint8_t i=0; i<3; i++) {
                if(v[i] > max[i]) max[i]=v[i];
                if(v[i] < min[i]) min[i]=v[i];
            }
        }

        return { .min=min, .max=max };
    }

    inline constexpr AABB ToAABB(Sphere sphere, glm::vec3 position) {
        return {};
    }

    inline constexpr AABB ToAABB(Capsule capsule) {
        return {};
    }

    inline constexpr AABB ToAABB(Capsule capsule, glm::mat4 const & transform) {
        return {};
    }

    inline constexpr AABB ToAABB(Mesh const & mesh) {
        return {};
    }

    inline constexpr AABB UpdateAABB(
        Mesh const & mesh,
        std::vector<std::uint32_t> const & indices) {
        return {};
    }

}
