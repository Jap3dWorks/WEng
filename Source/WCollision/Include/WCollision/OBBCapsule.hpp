#pragma once

#include "WCore/FloatingPoint.hpp"
#include "WCollision/Shapes.hpp"

#include <glm/glm.hpp>
#include <algorithm>
#include <limits>


namespace wcl::obb_capsule {

    struct LimitSign {
        std::int8_t Hx{0}; // -1, +1 or 0 if inside limits
        std::int8_t Hy{0};
        std::int8_t Hz{0};
    };

    inline float NearestSquareDistance(wcl::shapes::Box aabb, glm::vec3 p_a, glm::vec3 p_b) {
        glm::vec3 segment = p_b - p_a;

        std::array aabb_limits {
            -aabb.x, aabb.x,
            -aabb.y, aabb.y,
            -aabb.z, aabb.z
        };

        std::array<float, 3> constant_distance{-1};

        std::array<float, 6> t_limits{-1.f};
        std::uint32_t t_limits_count=0;
        for (std::uint32_t i = 0; i<3; i++) {
            if (wcr::fpoint::AreEqual(segment[i], 0.f)) {
                if (p_a[i] < aabb_limits[i]) {
                    constant_distance[i] = std::abs(p_a[i] - aabb_limits[i]);
                }
                else if (p_a[i] > aabb_limits[i+1]) {
                    constant_distance[i] = std::abs(p_a[i] - aabb_limits[i+1]);
                }
                else {
                    constant_distance[i]=0;
                }
            }
            else {
                for(std::uint32_t j=0; j<2; j++) {
                    float t = (aabb_limits[i * 2 + j] - p_a[i]) / (segment[i]);

                    if ( t>=0 && t <= 1.f ) {
                        t_limits[t_limits_count] = t;
                        ++t_limits_count;
                    }
                }
            }
        }

        std::sort(t_limits.begin(), t_limits.begin() + t_limits_count);

        std::array<float,8> t_intervals{1.f};
        t_intervals[0]=0.f;
        std::uint32_t t_intervals_count=1;
        
        if(t_limits_count > 0){
            std::uint32_t init=0;
            std::uint32_t stop=t_limits_count;
            if( wcr::fpoint::AreEqual(t_limits[0], 0.f) ) {
                init=1;
            }

            if (wcr::fpoint::AreEqual(t_limits[t_limits_count-1], 1.f)) {
                --stop;
            }

            stop = stop - init;

            for(std::uint32_t i=0; i < stop; i++) {
                t_intervals[i+1] = t_limits[i+init];
            }

            t_intervals[stop+1]=1.f;
            t_intervals_count=stop+1;
        }
        else{
            t_intervals[1]=1.f;
        }
        
        // TODO here

        // TODO detect nearest limit in each interval.
        std::array<LimitSign, 0> d_signs{};
        std::uint32_t d_signs_count=0;

        // check each t interval values

        float min_distance = std::numeric_limits<float>::max();

        for (std::uint32_t i=0; i<t_intervals_count; i++) {
            float t_0 = t_intervals[i];
            float t_1 = t_intervals[i+1];

            

        }

        return min_distance;

    }
    
}
