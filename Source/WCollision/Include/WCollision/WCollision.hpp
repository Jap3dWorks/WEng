#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WId.hpp"
#include "WColliders/Shapes.hpp"
#include "glm/ext/matrix_float4x4.hpp"

#include <optional>

#include <glm/glm.hpp>
#include <glm/matrix.hpp>

namespace wcl::collision {

    struct CollisionData{
        glm::vec3 hit_point{};
        
    };

    /**
     * Returns true if shapes are Axis Aligned Bounding Box.
     * Computing cheaper for a initial tests.
     */
    inline bool AABBIntersects(
        wcl::shapes::Cube a_shape, glm::vec3 a_position,
        wcl::shapes::Cube b_shape, glm::vec3 b_position
        ) {
        
        return false;
    }

    /**
     * @param b_transform : expected relative transform to a.
     */
    inline std::optional<CollisionData> Intersects(
        wcl::shapes::Cube a_shape, wcl::shapes::Cube b_shape, glm::mat4 b_transform
        ) {

        return std::nullopt;
    }

    /**
     * @param position : expected relative position to a.
     */
    inline std::optional<CollisionData> Intersects(
        wcl::shapes::Cube, wcl::shapes::Sphere, glm::vec3 position
        ) {
        return std::nullopt;
    }

    inline std::optional<CollisionData> Intersects(
        wcl::shapes::Cube, wcl::shapes::Capsule, glm::mat4
        ) {
        return std::nullopt;
    }

    inline std::optional<CollisionData> Intersects(
        wcl::shapes::Sphere, wcl::shapes::Sphere, glm::mat4
        ) {
        return std::nullopt;
    }
    
    inline std::optional<CollisionData> Intersects(
        wcl::shapes::Sphere, wcl::shapes::Capsule, glm::mat4
        ) {
        return std::nullopt;
    }

    inline std::optional<CollisionData> Intersects(
        wcl::shapes::Capsule, wcl::shapes::Capsule, glm::mat4
        ) {
        return std::nullopt;
    }

}
