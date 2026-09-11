#pragma once

#include "WCore/FloatingPoint.hpp"
#include "WCollision/Shapes.hpp"

#include <glm/glm.hpp>
#include <algorithm>
#include <limits>


namespace wcl::box_capsule {

    inline float MinSquareDistance(wcl::shapes::Box axis_box, glm::vec3 p_a, glm::vec3 p_b) {
        glm::vec3 segment = p_b - p_a;

        std::array abox_limits {
             axis_box.x,
             axis_box.y,
             axis_box.z
        };

        std::array<float, 3> const_sqr_dist{-1};
        std::array<bool, 3> is_constant{false};

        auto segment_point = [&p_a, &p_b]
            (float t_val) constexpr -> glm::vec3 {
            return p_a + (p_b - p_a) * t_val;
        };

        auto spoint_limit_sign = [&axis_box](glm::vec3 segment_point) constexpr
            {
                return std::array {
                    (segment_point.x < -axis_box.x)
                      ? std::int8_t{1}
                      : (segment_point.x > axis_box.x)
                        ? std::int8_t{-1}
                        : std::int8_t{0},
                    (segment_point.y < -axis_box.y)
                      ? std::int8_t{1}
                      : (segment_point.y > axis_box.y)
                        ? std::int8_t{-1}
                        : std::int8_t{0},
                    (segment_point.z < -axis_box.z)
                      ? std::int8_t{1}
                      : (segment_point.z > axis_box.z)
                        ? std::int8_t{-1}
                        : std::int8_t{0}
                };
            };

        auto get_sqr_dist =
            [&](float tval, auto & limit_sign) {
                glm::vec3 spoint = segment_point(tval);
                float value=0;
                for (std::uint32_t i=0; i<3; i++) {
                    if(is_constant[i])
                    {
                        value += const_sqr_dist[i];
                    }
                    else if (limit_sign[i] != 0) {
                        value += std::pow(spoint[i] + limit_sign[i] * abox_limits[i], 2);
                    }
                }

                return value;
            };

        std::array<float, 6> t_limits{-1.f};
        std::uint32_t t_limits_count=0;
        for (std::uint32_t i = 0; i<3; i++) {
            if (wcr::fpoint::AreEqual(segment[i], 0.f)) {
                is_constant[i]=true;
                if (p_a[i] < -abox_limits[i]) {
                    const_sqr_dist[i] = std::pow(p_a[i] + abox_limits[i], 2);
                }
                else if (p_a[i] > abox_limits[i]) {
                    const_sqr_dist[i] = std::pow(p_a[i] - abox_limits[i], 2);
                }
                else {
                    const_sqr_dist[i]=0;
                }
            }
            else {
                std::array signs = {1,-1};
                for(std::uint32_t j=0; j<2; j++) {
                    float t = (signs[j] * abox_limits[i] - p_a[i]) / (segment[i]);
                    if ( t>=0 && t <= 1.f ) {
                        t_limits[t_limits_count] = t;
                        ++t_limits_count;
                    }
                }
            }
        }

        std::sort(t_limits.begin(), t_limits.begin() + t_limits_count);

        std::array<float,7> t_intervals{1.f};
        t_intervals[0]=0.f;
        std::uint32_t t_intervals_count=1;
        
        if(t_limits_count > 0) {
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
            t_intervals_count= 1 + stop;
        }
        else{
            t_intervals[1]=1.f;
        }

        float min_square_dist = std::numeric_limits<float>::max();

        for (std::uint32_t i=0; i<t_intervals_count; i++) {
            float t0 = t_intervals[i];
            float t1 = t_intervals[i+1];
            float tmid = (t0 + t1) / 2.f;

            glm::vec3 spoint = segment_point(tmid);

            std::array limit_sign = spoint_limit_sign(spoint);

            float numerator=0;
            float denominator=0.f;
            bool all_0=true;

            for(std::uint32_t i=0; i<limit_sign.size(); i++) {
                if(limit_sign[i] != 0) {
                    numerator += -((p_a[i] + limit_sign[i] * abox_limits[i]) * segment[i]);
                    denominator += segment[i] * segment[i];
                    all_0 = false;
                }
            }

            if (all_0) {
                // segment inside AABB
                min_square_dist=0;
                break;
            }

            float tmin = std::min(std::max(numerator/denominator, t0), t1);

            glm::vec3 min_spoint=segment_point(tmin);

            float square_dist = get_sqr_dist(tmin, limit_sign);
            if(square_dist < min_square_dist) {
                min_square_dist = square_dist;
            }
        }

        return min_square_dist;

    }
    
}
