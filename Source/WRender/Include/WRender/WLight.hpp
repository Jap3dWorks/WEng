#pragma once

#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif

#include "WCoreTypes/WRenderTypes.hpp"
#include "WComponents/Transform.hpp"
#include "WComponents/Light/Point.hpp"
#include "WComponents/Light/Directional.hpp"
#include "WComponents/Light/Ambient.hpp"


#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

namespace wrd::light {

    inline constexpr wct::render::PointLight ToPointLight(
        glm::vec3 position,
        float radius,
        glm::vec3 color,
        float intensity
        ) {
        return {
            .color=color * intensity,
            .radius=radius,
            .position=position
        };
    }

    inline constexpr wct::render::DirectionalLight ToDirectionalLight(
        glm::vec3 direction,
        glm::vec3 color,
        float intensity
        )
    {
        return {
            .color=color * intensity,
            .direction=direction
        };
    }

    /**
     * Ortographic projectioon matrix
     */
    inline constexpr glm::mat4 ToShadowMapProjectionMatrix(
        float width, float height, float near=0.1f, float far=10.f
        ) {
        return glm::ortho(
            -(width * 0.5f),
            width * 0.5f,
            (height * 0.5f),
            -height * 0.5f,
            near,
            far
            );

        // return glm::ortho(
        //     0.f,
        //     width, 
        //     height,
        //     0.f,
        //     near,
        //     far
        //     );

    }

    inline constexpr glm::mat4 ToShadowMapViewMatrix(
        glm::mat4 transform_matrix,
        glm::vec3 interest_point={0.f, 0.f, 0.f}
        )
    {
        glm::mat3 orient{
            transform_matrix[2],
            transform_matrix[1],
            -transform_matrix[0]
        };

        assert(
            std::abs(glm::length(orient[0]) - 1.f) < 0.0001 &&
            std::abs(glm::length(orient[1]) - 1.f) < 0.0001 &&
            std::abs(glm::length(orient[2]) - 1.f) < 0.0001
            );

        glm::mat4 inv_orient=glm::transpose(orient);

        float radius = 10.f;
        glm::vec3 translation {interest_point - glm::vec3(transform_matrix[0]) * radius};

        glm::mat4 inv_translation = glm::translate(glm::mat4{1}, -translation);

        return inv_orient * inv_translation;
    }

    inline constexpr wct::render::AmbientLight ToAmbientLight(
        const wcm::light::Ambient & in_light
        )
    {
        return {
            .color=in_light.Get_color() * in_light.Get_intensity()
        };
    }


}
