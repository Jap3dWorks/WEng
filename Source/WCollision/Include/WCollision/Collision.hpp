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
     * @brief : If "a" and "b" are two Box collider shapes,
     * too ensure a valid inersection check with CheckOBBIntersection it is required to call
     * CheckOBBIntersection(a,b,b_transform_relative_to_a) && CheckOBBIntersection(b,a,a_transform_relative_to_b)
     */
    inline bool CheckOBBIntersection(
        wcl::shapes::Box center_box, wcl::shapes::Box other_box, glm::mat4 other_transform
        ) {
        auto other_radii = wcl::shapes::GetBoxRadii(other_box, other_transform);

        glm::vec3 axis_radii{0.f};

        for(std::uint32_t i=0; i<other_radii.size(); i++) {
            glm::vec3 abs_values = {
                std::abs(other_radii[i].x),
                std::abs(other_radii[i].y),
                std::abs(other_radii[i].z)
            };
                    
            if (abs_values.x > axis_radii.x) {
                axis_radii.x = abs_values.x;
            }
            if (abs_values.y > axis_radii.y) {
                axis_radii.y = abs_values.y;
            }
            if (abs_values.z > axis_radii.z) {
                axis_radii.z = abs_values.z;
            }
        }

        // X projection
        if (std::abs(other_transform[3].x) > center_box.x + axis_radii.x)
            return false;

        // Y projection
        if (std::abs(other_transform[3].y) > center_box.y + axis_radii.y)
            return false;

        // Z projection
        if (std::abs(other_transform[3].z) > center_box.z + axis_radii.z)
            return false;

        return true;
    }

    /**
     * Returns true if center_box and sphere are intersecting.
     * @param center_box : box collision shape, it is considered the center of the system.
     * @param sphere : sphere collision shape.
     * @param sphere_position : sphere position relative to center_box.
     */
    inline bool CheckIntersection(
        wcl::shapes::Box center_box,
        wcl::shapes::Sphere sphere,
        glm::vec3 sphere_position
        ) {
        sphere_position = {
            std::abs(sphere_position.x),
            std::abs(sphere_position.y),
            std::abs(sphere_position.z)
        };

        glm::vec3 nearest_box_point = glm::vec3{
            std::min(center_box.x, sphere_position.x),
            std::min(center_box.y, sphere_position.y),
            std::min(center_box.z, sphere_position.z)
        };

        glm::vec3 check = nearest_box_point - sphere_position;

        return glm::dot(check, check) <= sphere.radius * sphere.radius;
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
