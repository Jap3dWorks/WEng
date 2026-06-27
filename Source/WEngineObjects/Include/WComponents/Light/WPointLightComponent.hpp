#pragma once

#include "WEngineObjects/WComponent.hpp"
// #include "WCoreTypes/WRenderTypes.hpp"

#include <glm/glm.hpp>

#include "WPointLightComponent.WEngine.hpp"

namespace wcm::light {

    class WENGINEOBJECTS_API WPointLightComponent : public WComponent {

        WOBJECT_BODY;

        WPROPERTY(bool, active, true);
        WPROPERTY(float, intensity, 1.f);
        WPROPERTY(glm::vec3, color, glm::vec3(0.5, 0.5, 0.5));
        WPROPERTY(float, radius, 10.f);

    public:

    };
}
