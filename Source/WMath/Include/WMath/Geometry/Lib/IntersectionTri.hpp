#pragma once

#include "WMath/Geometry/IntersectionShape.hpp"
#include "WMath/Geometry/Shape.hpp"

#include <cstdint>

namespace wmath::geometry::lib::intersection_tri {
    
        inline constexpr std::uint8_t GetIsolatedVertex(
        wmath::geometry::shape::Tri tri,
        wmath::geometry::shape::Line line,
        glm::vec3 tri_normal
        ) {
        std::uint8_t m=0;
        glm::vec3 v = glm::cross(tri_normal, line.dir);

        for(std::uint8_t i=0; i<tri.size(); i++) {
            bool t = glm::dot(tri[i] - line.point, v) > 0.f;
            m |= static_cast<std::uint8_t>(t) << i;
        }
        
        switch(m) {
        case 1:
        case 6:
            return 0;
        case 2:
        case 5:
            return 1;
        case 3:
        case 4:
            return 2;
        default:
            // No isolated vertex found
            return 3;
        }
    }

    /**
     * @pre line and tri rests in the same plane.
     */
    inline constexpr std::optional<std::tuple<float,float>> TriLineSegment (
        wmath::geometry::shape::Tri tri,
        wmath::geometry::shape::Line line,
        std::uint8_t isolated_vertex
        ) {
        
        std::uint8_t i0 = isolated_vertex;
        std::uint8_t i1 = (i0 + 1) % 3;
        std::uint8_t i2 = (i0 + 2) % 3;

        wmath::geometry::shape::Line l1 {
            .dir=tri[i1] - tri[i0],
            .point=tri[i0]
        };

        auto l1_line_t =  wmath::geometry::intersection_shape::LineLine(
            l1, line
            );

        wmath::geometry::shape::Line l2 {
            .dir=tri[i2] - tri[i0],
            .point=tri[i0]
        };

        auto l2_line_t =  wmath::geometry::intersection_shape::LineLine(
            l2, line
            );

        if (l1_line_t.has_value() && l2_line_t.has_value() ) {
            return std::tuple{
                std::get<1>(l1_line_t.value()),
                std::get<1>(l2_line_t.value())
            };
        }
        else {
            return std::nullopt;
        }
    }

    inline constexpr bool SegmentsOverlap(
        wmath::geometry::shape::Segment s0,
        wmath::geometry::shape::Segment s1
        ) {

        

        return false;
    }

}
