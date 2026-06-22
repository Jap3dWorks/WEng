#pragma once

#include "WCoreTypes/WRenderTypes.hpp"
#include "WEngineObjects/WComponent.hpp"

#include <glm/glm.hpp>

#include "WDirectionalLightComponent.WEngine.hpp"

namespace wcm::light {

    class WENGINEOBJECTS_API WDirectionalLightComponent : public WComponent {

        WOBJECT_BODY;

        WPROPERTY(bool, active, true);
        WPROPERTY(float, intensity, 1.f);
        WPROPERTY(glm::vec4, color, glm::vec4(0.5, 0.5, 0.5, 1.0));

    public:

        // Not sure if this is the right place,
        // I cant get the direction from inside the component.
        WNODISCARD wct::render::WDirectionalLight ToDirectionalLight() const {
            return {
                .color=color,
                .intensity=intensity
            };
        }

    };
    
}
