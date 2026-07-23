#pragma

#include "WCoreTypes/WRenderTypes.hpp"
#include "WComponents/Transform.hpp"
#include "WComponents/Light/Point.hpp"
#include "WComponents/Light/Directional.hpp"
#include "WComponents/Light/Ambient.hpp"

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

    inline constexpr wct::render::AmbientLight ToAmbientLight(
        const wcm::light::Ambient & in_light
        )
    {
        return {
            .color=in_light.Get_color() * in_light.Get_intensity()
        };
    }
}
