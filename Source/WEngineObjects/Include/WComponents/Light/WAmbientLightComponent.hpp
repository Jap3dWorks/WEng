#pragma once

#include "WEngineObjects/WComponent.hpp"

#include <glm/glm.hpp>

#include "WAmbientLightComponent.WEngine.hpp"

namespace wcm::light {
    
    class WENGINEOBJECTS_API WAmbientLightComponent : public WComponent {

        WOBJECT_BODY;

        WPROPERTY(bool, active, true);
        WPROPERTY(float, intensity, 1.f);
        WPROPERTY(glm::vec4, color, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
    };

}
