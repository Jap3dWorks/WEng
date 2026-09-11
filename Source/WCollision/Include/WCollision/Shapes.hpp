#pragma once

#include <variant>
#include <glm/glm.hpp>

namespace wcl::shapes {

    struct AABB {
        glm::vec3 min{-5.f, -5.f, -5.f};
        glm::vec3 max{5.f, 5.f, 5.f};
    };

    struct Box{
        float x{5.f};  // half side length
        float y{5.f};
        float z{5.f};
    };

    struct Sphere{
        float radius{5.f};
    };

    struct Capsule{
        float half_length{5.f};  // half segment length
        float radius{3.f};
    };
    
    using ShapeVariant = std::variant<Box, Sphere, Capsule>;

    inline constexpr std::array<glm::vec3,8> GetBoxVertices(Box cube, glm::mat4 cube_transform) {

        std::array<glm::vec3, 8> result{};

        result[0] = cube_transform * glm::vec4{cube.x, cube.y, cube.z, 1.f};
        result[1] = cube_transform * glm::vec4{-cube.x, cube.y, cube.z, 1.f};
        result[2] = cube_transform * glm::vec4{cube.x, -cube.y, cube.z, 1.f};
        result[3] = cube_transform * glm::vec4{cube.x, cube.y, -cube.z, 1.f};
        result[4] = cube_transform * glm::vec4{-cube.x, -cube.y, cube.z, 1.f};
        result[5] = cube_transform * glm::vec4{-cube.x, cube.y, -cube.z, 1.f};
        result[6] = cube_transform * glm::vec4{cube.x, -cube.y, -cube.z, 1.f};
        result[7] = cube_transform * glm::vec4{-cube.x, -cube.y, -cube.z, 1.f};

        return result;
    }

    inline constexpr AABB ToAABB(Box box) {
        return {
            .min{-box.x, -box.y, -box.z},
            .max{box.x, box.y, box.z}
        };
    }

    inline constexpr std::array<glm::vec3,8> GetBoxRadii(Box box, glm::mat4 cube_transform) {

        std::array<glm::vec3,8> result{};

        glm::mat3 rot = cube_transform;

        result[0] = rot * glm::vec3{box.x, box.y, box.z};
        result[1] = rot * glm::vec3{-box.x, box.y, box.z};
        result[2] = rot * glm::vec3{box.x, -box.y, box.z};
        result[3] = rot * glm::vec3{box.x, box.y, -box.z};
        result[4] = rot * glm::vec3{-box.x, -box.y, box.z};
        result[5] = rot * glm::vec3{-box.x, box.y, -box.z};
        result[6] = rot * glm::vec3{box.x, -box.y, -box.z};
        result[7] = rot * glm::vec3{-box.x, -box.y, -box.z};

        return result;
        
    }

    inline constexpr auto AsPoints(Capsule capsule) {
        return std::array{
            glm::vec3{-capsule.half_length, 0.f, 0.f},
            glm::vec3{capsule.half_length, 0.f, 0.f}
        };
    }

    inline constexpr auto AsPoints(Capsule capsule, glm::mat4 transform) {
        auto [p_a, p_b] = wcl::shapes::AsPoints(capsule);
        
        return std::array {glm::vec3{transform * glm::vec4{p_a, 1.f}},
                           glm::vec3{transform * glm::vec4{p_b, 1.f}}};

    }
}
