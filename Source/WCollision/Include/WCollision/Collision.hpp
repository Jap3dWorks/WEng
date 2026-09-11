#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WId.hpp"
#include "WCollision/Shapes.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "WCollision/BoxCapsule.hpp"

// #include <optional>

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
    inline bool CheckBoxIntersection(
        wcl::shapes::Box axis_box, wcl::shapes::Box obb, glm::mat4 obb_transform
        ) {
        auto other_radii = wcl::shapes::GetBoxRadii(obb, obb_transform);

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
        if (std::abs(obb_transform[3].x) > axis_box.x + axis_radii.x)
            return false;

        // Y projection
        if (std::abs(obb_transform[3].y) > axis_box.y + axis_radii.y)
            return false;

        // Z projection
        if (std::abs(obb_transform[3].z) > axis_box.z + axis_radii.z)
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
        wcl::shapes::Box axis_box,
        wcl::shapes::Sphere sphere,
        glm::vec3 sphere_position
        ) {
        sphere_position = {
            std::abs(sphere_position.x),
            std::abs(sphere_position.y),
            std::abs(sphere_position.z)
        };

        glm::vec3 nearest_box_point = glm::vec3{
            std::min(axis_box.x, sphere_position.x),
            std::min(axis_box.y, sphere_position.y),
            std::min(axis_box.z, sphere_position.z)
        };

        glm::vec3 check = nearest_box_point - sphere_position;

        return glm::dot(check, check) <= sphere.radius * sphere.radius;
    }

    inline bool CheckIntersection(
        wcl::shapes::Box axis_box, wcl::shapes::Capsule capsule, glm::mat4 capsule_transform
        ) {

        auto [p_a, p_b] = wcl::shapes::AsPoints(capsule, capsule_transform);

        return wcl::box_capsule::MinSquareDistance(axis_box, p_a, p_b) <=
            (capsule.radius * capsule.radius);
    }

    inline bool CheckIntersection(
        wcl::shapes::Sphere a_sphere, wcl::shapes::Sphere b_sphere, glm::vec3 b_translation
        ) {

        float sqr_dist = glm::dot(b_translation, b_translation);

        return std::pow(a_sphere.radius + b_sphere.radius, 2) >= sqr_dist;
    }

    /**
     * @param capsule_transform : capsule transform relative to axis_sphere.
     */
    inline bool CheckIntersection(
        wcl::shapes::Sphere axis_sphere, wcl::shapes::Capsule capsule, glm::mat4 capsule_transform
        ) {

        auto [p_a, p_b] = wcl::shapes::AsPoints(capsule, capsule_transform);

        glm::vec3 segment = p_b - p_a;

        float t = (- glm::dot(p_a, segment))/(glm::dot(segment, segment));

        float t_min = std::max(std::min(t,1.f), 0.f);

        glm::vec3 spoint = p_a + (p_b - p_a) * t;

        return glm::dot(spoint, spoint) <= std::pow(axis_sphere.radius + capsule.radius, 2);
    }

    inline bool CheckIntersection(
        wcl::shapes::Capsule axis_capsule,
        wcl::shapes::Capsule capsule,
        glm::mat4 capsule_transform
        ) {

        auto[p1_a, p1_b] = wcl::shapes::AsPoints(axis_capsule);
        auto[p2_a, p2_b] = wcl::shapes::AsPoints(capsule, capsule_transform);

        glm::vec3 d1 = p1_b - p1_a;
        glm::vec3 d2 = p2_b = p2_a;

        float d2_dt = glm::dot(d2,d2);
        float d1_dt = glm::dot(d1,d1);

        float p1d1_dt = glm::dot(p1_a, d1);
        float p2d2_dt = glm::dot(p2_a, d2);
        float p1d2_dt = glm::dot(p1_a, d2);
        float p2d1_dt = glm::dot(p2_a,d1);

        float d1d2_dt = glm::dot(d1,d2);

        float t_nearest =
            (d1_dt * (p1d2_dt - p2d2_dt) - p1d2_dt + p2d1_dt) /
            ((d2_dt * d1_dt) - d1d2_dt);

        float t_min = std::max(std::min(t_nearest, 1.f), 0.f);

        float s_nearest =
            (- p1d1_dt + p2d1_dt + t_min * d1d2_dt) / d1_dt;

        float s_min = std::max(std::min(s_nearest, 1.f), 0.f);

        return glm::dot(
            p1_a + d1 * s_min, p2_a + d2 * t_min
            ) <= std::pow(axis_capsule.radius + capsule.radius,2);
    }

}
