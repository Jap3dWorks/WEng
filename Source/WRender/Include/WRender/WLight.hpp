#pragma once

#include <glm/ext/matrix_clip_space.hpp>

#include "WCoreTypes/WRenderTypes.hpp"
#include "WComponents/Transform.hpp"
#include "WComponents/Light/Point.hpp"
#include "WComponents/Light/Directional.hpp"
#include "WComponents/Light/Ambient.hpp"

#include <glm/glm.hpp>
#include <glm/matrix.hpp>

namespace wrd::light {

    inline constexpr wct::render::PointLight ToPointLight(
        const wcm::Transform & in_transform,
        const wcm::light::Point & in_light
        ) {
        return {
            .color={in_light.Get_color() * in_light.Get_intensity()},
            .radius=in_light.Get_radius(),
            .position={in_transform.Get_position()}
        };
    }

    inline constexpr wct::render::DirectionalLight ToDirectionalLight(
        const wcm::Transform & in_transform,
        const wcm::light::Directional & in_light
        )
    {
        return {
            .color=in_light.Get_color() * in_light.Get_intensity(),
            .direction=in_transform.Get_transform_matrix()[0]
        };
    }

    inline constexpr glm::mat4 ToShadowMapProjectionMatrix(
        wcm::light::Directional const & directional,
        float size
        ) {
        return glm::ortho(
            -1 * size, 1 * size, -1 * size, 1 * size
            );
    }

    inline constexpr glm::mat4 ToShadowMapViewMatrix(
        wcm::Transform const & transform,
        wcm::light::Directional const & directional,
        glm::vec3 interest_point={0.f, 0.f, 0.f}
        )
    {
        glm::mat4 trns = transform.Get_transform_matrix();

        glm::mat3 orient{trns[2], trns[1], -trns[0]};

        // assert if it is and ortogonal matrix
        assert(
            std::abs(orient[0].length() - 1.f) < 0.00001 &&
            std::abs(orient[1].length() - 1.f) < 0.00001 &&
            std::abs(orient[2].length() - 1.f) < 0.00001
            );

        glm::mat4 inv_orient=glm::transpose(orient);

        float radius = 2500.f;
        glm::vec3 translation {interest_point - glm::vec3(trns[0]) * radius};

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
