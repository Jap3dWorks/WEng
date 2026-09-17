#pragma once

#include "WMath/Geometry/Shapes.hpp"

#include <glm/glm.hpp>

namespace wcl::closest_point {

    inline constexpr glm::vec3 OnTriangle(
        std::array<glm::vec3, 3> tri,
        glm::vec3 point
        ) {

        // Check if point is outside vertex 0
        glm::vec3 ab = tri[0] - tri[1];
        glm::vec3 ac = tri[0] - tri[2];
        glm::vec3 ap = tri[0] - point;

        float d1 = glm::dot(ab, ap);
        float d2 = glm::dot(ac, ap);
        if (d1 <= 0.f && d2 <= 0.f) return tri[0];

        // Check if point is outside vertex 1
        glm::vec3 bp = point - tri[1];
        float d3 = glm::dot(ab, bp);
        float d4 = glm::dot(ac, bp);
        if (d3 <= 0.f && d4 <= d3) return tri[1];

        // Check if point in region ab
        float vc = d1 * d4 - d3 * d2;
        if (vc <= 0.f && d1 >= 0.f && d3 <= 0.f) {
            float v = d1 / (d1 - d3);
            return tri[0] + v * ab;
        }

        // Check if point in vertex region outside c
        glm::vec3 cp = point - tri[2];
        float d5 = glm::dot(ab, cp);
        float d6 = glm::dot(ac, cp);
        if (d6 >= 0.f && d5 <= d6) return tri[2];

        // Check if point in edge region AC
        float vb = d5 * d2 - d1 * d6;
        if (vb <=0.f && d2 <= 0.f && d6 <= 0.f) {
            float w = d2 / (d2 - d6);
            return tri[0] + w * ac;
        }

        // Check region BC
        float va = d3*d6 - d5*d4;
        if (va==0.f && (d4 - d3) >= 0.f && (d5 - d6) >= 0.f) {
            float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
            return tri[1] + w * (tri[2] - tri[1]);
        }

        // point inside face region
        float denom = 1.f / (va + vb + vc);
        float v = vb * denom;
        float w = vc * denom;
        
        return tri[0] + ab * v + ac * w;
    }

    inline constexpr glm::vec3 OnPlane(
        wcl::shapes::Plane plane,
        glm::vec3 point
        ) {
        float l = plane.dist -  (
            glm::dot(point, plane.n) / glm::dot(plane.n, plane.n)
            );

        return point + plane.n * l;
    }
    

    /**
     * @brief Returns {s_min, t_min}
     * so s0.p0 + (s0.p1 - s0.p0) * s_min is the nearest point to s1 on s0,
     * and s1.p0 + (s1.p1 - s1.p0) * t_min is the nearest point to s0 on s1.
     */
    inline constexpr std::array<float,2> OnSegments(
        wcl::shapes::Segment s0,
        wcl::shapes::Segment s1
        ) {

        glm::vec3 d1 = s0.p1 - s0.p0;
        glm::vec3 d2 = s1.p1 - s1.p0;

        float d2_dt = glm::dot(d2,d2);
        float d1_dt = glm::dot(d1,d1);

        float p1d1_dt = glm::dot(s0.p0 /* p1_a */, d1);
        float p2d2_dt = glm::dot(s1.p0 /* p2_a */, d2);
        float p1d2_dt = glm::dot(s0.p0 /* p1_a */, d2);
        float p2d1_dt = glm::dot(s1.p0 /* p2_a */,d1);

        float d1d2_dt = glm::dot(d1,d2);

        float t_nearest =
            (d1_dt * (p1d2_dt - p2d2_dt) - p1d2_dt + p2d1_dt) /
            ((d2_dt * d1_dt) - d1d2_dt);

        float t_min = std::max(std::min(t_nearest, 1.f), 0.f);

        float s_nearest =
            (- p1d1_dt + p2d1_dt + t_min * d1d2_dt) / d1_dt;

        float s_min = std::max(std::min(s_nearest, 1.f), 0.f);

        return std::array{s_min, t_min};
    }

    

}
