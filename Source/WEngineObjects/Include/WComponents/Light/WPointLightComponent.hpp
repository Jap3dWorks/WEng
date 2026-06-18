#pragma once

#include "WEngineObjects/WComponent.hpp"

#include <glm/glm.hpp>

#include "WPointLightComponent.WEngine.hpp"

namespace wcm::light {

    class WENGINEOBJECTS_API WPointLightComponent : public WComponent {

        WOBJECT_BODY;

        WPROPERTY(bool, active, true);
        WPROPERTY(float, intensity, 1.f);
        WPROPERTY(glm::vec4, color, glm::vec4(0.5, 0.5, 0.5, 1.0));
        WPROPERTY(float, radius, 10.f);

    };
}
