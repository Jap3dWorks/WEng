#pragma once

#include "WCore/FloatingPoint.hpp"
#include "WCollision/Shapes.hpp"

#include "glm/glm.hpp"

namespace wcl::obb_capsule {

    struct LimitSign {
        std::int8_t Hx{0};
        std::int8_t Hy{0};
        std::int8_t Hz{0};
    };

    void TIntervals(wcl::shapes::Box aabb, glm::vec3 p_a, glm::vec3 p_b) {

        glm::vec3 segment = p_b - p_a;

        std::array aabb_limits {
            -aabb.x, aabb.x,
            -aabb.y, aabb.y,
            -aabb.z, aabb.z
        };

        std::array distances {
            -1.f, -1.f,
            -1.f, -1.f,
            -1.f, -1.f
        };

        std::array<std::uint8_t, 3> constant_axis;

        std::array<float, 6> t_limits{-1.f};
        std::uint32_t t_count=0;

        std::array<LimitSign, 6> d_signs{};

        for(std::uint32_t i=0; i< aabb_limits.size(); i++) {

            if (wcr::fpoint::AreEqual(segment[i/2], 0.f)) {
                constant_axis[i/2]=1;

            } else {

                float t = (aabb_limits[i] - p_a[i / 2]) / (segment[i/2]);

                if ( t>=0 && t <= 1.f ) {
                    // valid segment point
                    t_limits[t_count] = t;
                    ++t_count;
                }
            }
        }


        // glm::vec3 segment_point = p_a + (p_b - p_a)*t;

        // d_signs[t_count].Hx =
        //     (segment_point.x < -aabb.x) ? -1 : (segment_point.x > aabb.x) ? 1 : 0;
        // d_signs[t_count].Hy =
        //     (segment_point.y < -aabb.y) ? -1 : (segment_point.y > aabb.y) ? 1 : 0;
        // d_signs[t_count].Hz =
        //     (segment_point.z < -aabb.z) ? -1 : (segment_point.z > aabb.z) ? 1 : 0;



        

    }
    
}
