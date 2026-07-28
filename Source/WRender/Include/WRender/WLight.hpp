#include <glm/ext/matrix_clip_space.hpp>
#pragma

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
        wcm::light::Directional const & directional
        )
    {
        glm::mat4 trns = transform.Get_transform_matrix();

        // -pi/2 y rotate, so z is looking back
        glm::mat4 mat {
            trns[2], trns[1], -trns[0], trns[3]
        };

        glm::mat3 orient{mat};

        // ortogonal matrix traspose is equivalent to inverse
        glm::mat4 inv_orient=glm::transpose(orient);

        glm::vec3 translation {mat[3]};

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
