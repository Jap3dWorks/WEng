#pragma once

#include <glm/glm.hpp>

namespace wcl::triangle {

    inline glm::vec3 ClosestPoint(
        glm::vec3 point,
        std::array<glm::vec3, 3> tri) {

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
}
