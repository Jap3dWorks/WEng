#pragma

#include "WCoreTypes/WRenderTypes.hpp"
#include "WComponents/WTransformComponent.hpp"
#include "WComponents/Light/WPointLightComponent.hpp"
#include "WComponents/Light/WDirectionalLightComponent.hpp"
#include "WComponents/Light/WAmbientLightComponent.hpp"

namespace wrd::light {
    inline constexpr wct::render::WPointLight ToPointLight(
        const WTransformComponent & in_transform,
        const wcm::light::WPointLightComponent & in_light
        ) {
        return {
            .color=in_light.Get_color(),
            .position={in_transform.Get_position(), 1.f},
            .intensity=in_light.Get_intensity(),
            .radius=in_light.Get_radius(),
        };
    }

    inline constexpr wct::render::WDirectionalLight ToDirectionalLight(
        const WTransformComponent & in_transform,
        const wcm::light::WDirectionalLightComponent & in_light
        )
    {
        return {};
    }

    inline constexpr wct::render::WAmbientLight ToAmbientLight(
        const wcm::light::WAmbientLightComponent & in_light
        )
    {
        return {};
    }
}
