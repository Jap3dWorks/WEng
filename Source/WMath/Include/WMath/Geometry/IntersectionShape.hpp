#pragma once

#include "WMath/Geometry/Shape.hpp"
#include "WMath/Numerical.hpp"

#include <glm/glm.hpp>
#include <optional>
#include <cassert>

namespace {

    inline constexpr auto GetNotNULLMask(
        glm::vec3 va,
        glm::vec3 vb
        ) {
        std::uint8_t mask_a = 0;
        std::uint8_t mask_b = 0;

        for (std::uint8_t i=0; i<3; i++) {
            mask_a |= (!wmath::numerical::AreEqual(va[i], 0.f));
            mask_b |= (!wmath::numerical::AreEqual(vb[i], 0.f));
        }

        bool swap=false;

        if (mask_b < mask_a) {
            std::swap(mask_a, mask_b);
            swap=true;
        }

        return std::tuple{swap, mask_a, mask_b};
    }

    inline constexpr auto GetNotNULLMask(
        wmath::geometry::shape::Plane pa,
        wmath::geometry::shape::Plane pb
        ) {
        auto [doswap, mask_a, mask_b] = GetNotNULLMask(pa.n, pb.n);
        if (doswap) std::swap(pa,pb);

        return std::tuple{pa, pb, mask_a, mask_b};
    }

    inline constexpr auto GetNotNULLMask(
        wmath::geometry::shape::Line la,
        wmath::geometry::shape::Line lb
        ) {
        auto [doswap, mask_a, mask_b] = GetNotNULLMask(la.dir, lb.dir);
        if (doswap) std::swap(la,lb);

        return std::tuple{la, lb, mask_a, mask_b};
    }

    /**
     * @pre mask_a <= mask_b
     */
    inline constexpr auto GetAxisIndexFromNotNULLMask (
        std::uint8_t mask_a,
        std::uint8_t mask_b
        ) {
        assert(mask_a <= mask_b);
        
        if (mask_a & 1){
            if(mask_b & 2) {
                return std::tuple{0,1};
            }
            else {
                return std::tuple{0,2};
            }
        }
        else {
            return std::tuple{1,2};
        }
    }
    

}

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
            plane.n.x * (plane.offset * plane.n.x - segment.p0.x) +
            plane.n.y * (plane.offset * plane.n.y - segment.p0.y) +
            plane.n.z * (plane.offset * plane.n.z - segment.p0.z)
            ) / D;

        if (t<0.f || t>1.f) return std::nullopt;

        return segment.p0 + l * t;
    }

    inline constexpr std::optional<wmath::geometry::shape::Line> PlanePlane(
        wmath::geometry::shape::Plane p0,
        wmath::geometry::shape::Plane p1
        ) {
        auto find_line = [&p0, &p1] () -> std::optional<wmath::geometry::shape::Line> {
            float denom = (p0.n.x * p1.n.y - p1.n.x * p0.n.y);

            float u = p0.ConstantTerm();
            float v = p1.ConstantTerm();

            if (wmath::numerical::AreEqual(denom, 0.f)) {
                // parallel planes
                return std::nullopt;
            }

            return wmath::geometry::shape::Line {
                .dir = {
                    - (p0.n.x * p1.n.y * p1.n.z - p1.n.z * p0.n.y),
                    - ((p0.n.x * p1.n.z - p1.n.x * p0.n.z) / denom),
                    1.f
                },
                .point = {
                    p1.n.y * u - v * p0.n.y,
                    (p0.n.x * v - p1.n.x * u) / denom,
                    0.f
                }
            };
        };

        auto find_line_2 = [] (
            wmath::geometry::shape::Plane const & pa,
            wmath::geometry::shape::Plane const & pb,
            std::uint8_t axis_0,
            std::uint8_t axis_1
            ) -> std::optional<wmath::geometry::shape::Line> {
            glm::vec3 p{0.f};
                
            float u = pa.ConstantTerm();
            float v = pb.ConstantTerm();

            float denom = pb.n[axis_1]*pa.n[axis_0] - pa.n[axis_0] * pb.n[axis_1];

            if (wmath::numerical::AreEqual(denom, 0.f))
                return std::nullopt;

            float num = v * pa.n[axis_0] - u*pb.n[axis_0];

            p[axis_0] =
            u/pa.n[axis_0] -
            (pa.n[axis_1]/pa.n[axis_0]) * (num/denom);

            p[axis_1] = num/denom;

            switch(axis_0 + axis_1) {
            case 1:
                return wmath::geometry::shape::Line {
                    .dir=glm::vec3{0.f, 0.f, 1.f},
                    .point=p
                };
            case 2:
                return wmath::geometry::shape::Line {
                    .dir=glm::vec3{0.f, 1.f, 0.f},
                    .point=p
                };
            case 3:
                return wmath::geometry::shape::Line {
                    .dir=glm::vec3{1.f, 0.f, 0.f},
                    .point=p
                };
            }

            return std::nullopt;
        };

        auto [pa, pb, mask_a, mask_b] = GetNotNULLMask(p0, p1);
        
        auto [axis_1, axis_2] =  GetAxisIndexFromNotNULLMask(mask_a, mask_b);

        switch(mask_a | mask_b) {
        case 5:
        case 6:
        case 3:
            return find_line_2(pa, pb, axis_1, axis_2);
        case 7:
            return find_line();
        default:
            // parallel planes
            return std::nullopt;
        }
    }

    /**
     * @returns (ta, tb) values for line_a and line_b,
     * check wmath::geometry::shape::Line::Point method.
     */
    inline constexpr std::optional<std::tuple<float,float>> LineLine(
        wmath::geometry::shape::Line line_a,
        wmath::geometry::shape::Line line_b
        ) {

        auto line_t_values = [] (
            wmath::geometry::shape::Line la,
            wmath::geometry::shape::Line lb,
            std::uint8_t axis_0,
            std::uint8_t axis_1
            ) -> std::optional<std::tuple<float, float>>
            {
                float denom = la.dir[axis_1] * lb.dir[axis_0] - lb.dir[axis_1] * la.dir[axis_0];

                if (wmath::numerical::AreEqual(denom, 0.f)) {
                    // Parallel
                    return std::nullopt;
                }

                float tb = (
                    la.dir[axis_1] * (lb.point[axis_0] - la.point[axis_0]) +
                    lb.point[axis_1] * la.dir[axis_0]
                    ) / denom;

                float ta = (
                    lb.dir[axis_0] * tb + lb.point[axis_0] - la.point[axis_0]
                    ) / la.dir[axis_0];

                return std::tuple{ta, tb};
            };

        auto [la, lb, mask_a, mask_b] = GetNotNULLMask(line_a, line_b);
        auto [axis_0, axis_1] = GetAxisIndexFromNotNULLMask(mask_a, mask_b);

        std::optional<std::tuple<float,float>> t_ab;

        switch(mask_a | mask_b) {
        case 5:
        case 6:
        case 3:
        case 7:
            t_ab = line_t_values(la, lb, axis_0, axis_1);
            break;
        default:
            // parallel lines
            return std::nullopt;
        }

        if (!t_ab.has_value()) return std::nullopt;

        std::uint8_t axis_r=3;

        switch(axis_0 + axis_1) {
        case 1:
            axis_r = 2;
            break;
        case 2:
            axis_r = 1;
            break;
        case 3:
            axis_r = 0;
            break;
        }

        glm::vec3 point_a = la.Point(std::get<0>(t_ab.value()));
        glm::vec3 point_b = lb.Point(std::get<1>(t_ab.value()));

        if (wmath::numerical::AreEqual(point_a[axis_r], point_b[axis_r])) {
            return t_ab;
        }

        else {
            return std::nullopt;
        }

    }


}
