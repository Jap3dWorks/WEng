#pragma once

#include "WMath/Geometry/Shape.hpp"
#include "WMath/Numerical.hpp"

#include <glm/glm.hpp>
#include <optional>

namespace wmath::geometry::intersection_shape {

    inline constexpr std::optional<glm::vec3> PlaneSegment(
        wmath::geometry::shape::Plane plane,
        wmath::geometry::shape::Segment segment
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

    inline constexpr std::optional<wmath::geometry::shape::Line> PlanePlane(
        wmath::geometry::shape::Plane p0,
        wmath::geometry::shape::Plane p1
        ) {
        float u0 = glm::dot(p0.n, p0.n) * p0.dist;
        float u1 = glm::dot(p1.n, p1.n) * p1.dist;

        float denom = p0.n.x * p0.n.z - p1.n.x * p0.n.y;

        if (wmath::numerical::AreEqual(denom, 0.f)) {
            // planes are parallel
            return std::nullopt;
        }

        glm::vec3 n {
            p0.n.x * (p1.n.y * p0.n.z - p1.n.z * p0.n.y),
            (p0.n.x * p1.n.z - p1.n.x * p0.n.z) / denom,
            1.f
        };

        // glm::vec3 p {
        //     ()
        // }

    }

}
