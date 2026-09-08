#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WId.hpp"
#include "WCollision/Shapes.hpp"
#include "glm/ext/matrix_float4x4.hpp"

#include <optional>

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <limits>

namespace wcl::collision {

    /**
     * Returns true if shapes are Axis Aligned Bounding Box.
     * Computing cheaper for a initial tests.
     */
    inline bool CheckAABB(
        wcl::shapes::AABB a,
        wcl::shapes::AABB b
        ) {
        if (a.max[0] < b.min[0] || a.min[0] > b.max[0]) return false;
        if (a.max[1] < b.min[1] || a.min[1] > b.max[1]) return false;
        if (a.max[2] < b.min[2] || a.min[2] > b.max[2]) return false;

        return true;
    }

    /**
     * @param b_transform : expected relative transform to a.
     */
    inline bool CheckIntersection(
        wcl::shapes::Box a_shape, wcl::shapes::Box b_shape, glm::mat4 b_transform
        ) {

        auto b_points = wcl::shapes::GetBoxVertices(b_shape, b_transform);
        std::uint32_t p_index=9;

        auto get_nearest =
            [&b_points]
            (std::uint32_t axis) constexpr -> std::uint32_t  {
                std::uint32_t result=9;
                float dist=std::numeric_limits<float>::max();

                for(std::uint32_t i=0; i<b_points.size(); i++) {
                    if (std::abs(b_points[i].y) < dist) {
                        dist = std::abs(b_points[i].y);
                        result=i;
                    }
                }
                
                return result;
            };

        // X projection
        p_index = get_nearest(0);
        if (b_transform[3][0] > a_shape.x + (b_transform[3].x - b_points[p_index][0]))
            return false;

        // Y projection
        p_index = get_nearest(1);
        if (b_transform[3][1] > a_shape.y + (b_transform[3].y - b_points[p_index][1]))
            return false;

        // Z projection
        p_index = get_nearest(1);
        if (b_transform[3][2] > a_shape.z + (b_transform[3].z - b_points[p_index][2]))
            return false;

        return true;
    }

    /**
     * @param position : expected relative position to a.
     */
    inline bool CheckIntersection(
        wcl::shapes::Box, wcl::shapes::Sphere, glm::vec3 position
        ) {
        return false;
    }

    inline bool CheckIntersection(
        wcl::shapes::Box, wcl::shapes::Capsule, glm::mat4
        ) {
        return false;
    }

    inline bool CheckIntersection(
        wcl::shapes::Sphere, wcl::shapes::Sphere, glm::mat4
        ) {
        return false;
    }
    
    inline bool CheckIntersection(
        wcl::shapes::Sphere, wcl::shapes::Capsule, glm::mat4
        ) {
        return false;
    }

    inline bool CheckIntersection(
        wcl::shapes::Capsule, wcl::shapes::Capsule, glm::mat4
        ) {
        return false;
    }

}
