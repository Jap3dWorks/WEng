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

        auto axis_index = [](std::uint8_t mask_a, std::uint8_t mask_b) {
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
        };

        wmath::geometry::shape::Plane pa=p0;
        wmath::geometry::shape::Plane pb=p0;

        std::uint8_t mask_a = 0;
        std::uint8_t mask_b = 0;

        for (std::uint8_t i=0; i<3; i++) {
            mask_a |= wmath::numerical::AreEqual(p0.n[i], 0.f);
            mask_b |= wmath::numerical::AreEqual(p1.n[i], 0.f);
        }

        if (mask_b < mask_a) {
            std::swap(mask_a, mask_b);
            std::swap(pa, pb);
        }

        auto [axis_1, axis_2] = axis_index(mask_a, mask_b);

        switch(mask_a | mask_b) {
        case 5:
        case 6:
        case 3:
            return find_line_2(pa, pb, axis_1, axis_2);
        case 7:
            return find_line();
        default:
            // parellel planes
            return std::nullopt;
        }
    }


}
