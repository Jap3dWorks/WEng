#pragma

#include "WCoreTypes/WRenderTypes.hpp"
#include "WComponents/WTransformComponent.hpp"
#include "WComponents/Light/WPointLightComponent.hpp"
#include "WComponents/Light/WDirectionalLightComponent.hpp"
#include "WComponents/Light/WAmbientLightComponent.hpp"

namespace wrd::light {

    inline constexpr wct::render::PointLight ToPointLight(
        const WTransformComponent & in_transform,
        const wcm::light::WPointLightComponent & in_light
        ) {
        return {
            .color={in_light.Get_color() * in_light.Get_intensity()},
            .radius=in_light.Get_radius(),
            .position={in_transform.Get_position()}
        };
    }

    inline constexpr wct::render::DirectionalLight ToDirectionalLight(
        const WTransformComponent & in_transform,
        const wcm::light::WDirectionalLightComponent & in_light
        )
    {
        return {
            .color=in_light.Get_color() * in_light.Get_intensity(),
            .direction=in_transform.Get_transform_matrix()[0]
        };
    }

    inline constexpr wct::render::AmbientLight ToAmbientLight(
        const wcm::light::WAmbientLightComponent & in_light
        )
    {
        return {
            .color=in_light.Get_color() * in_light.Get_intensity()
        };
    }
}
