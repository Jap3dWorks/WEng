#pragma once

#include "WMath/Geometry/Shapes.hpp"
#include "WMath/Numerical.hpp"

#include <glm/glm.hpp>
#include <optional>

namespace wcl::intersection_point {

    inline constexpr std::optional<glm::vec3> PlaneSegment(
        wcl::shapes::Plane plane,
        wcl::shapes::Segment segment
        ) {

        glm::vec3 l = segment.p1 - segment.p0;
        float D = glm::dot(plane.n, l);

        if (wmath::numerical::AreEqual(D, 0.f)) {
            return std::nullopt;
        }

        float t = (
            plane.n.x * (plane.dist * plane.n.x - segment.p0.x) +
            plane.n.y * (plane.dist * plane.n.y - segment.p0.y) +
            plane.n.z * (plane.dist * plane.n.z - segment.p0.z)
            ) / D;

        if (t<0.f || t>1.f) return std::nullopt;

        return segment.p0 + l * t;
    }

}
