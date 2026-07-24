#pragma once

#include "WCoreTypes/WRenderTypes.hpp"
#include "WObjects/WComponent.hpp"

#include <glm/glm.hpp>

#include "wcm::light::Directional.WEng.hpp"

namespace wcm::light {

    class WOBJECTS_API Directional : public WComponent {

        WOBJECT_BODY;

        WPROPERTY(bool, active, true);
        WPROPERTY(float, intensity, 1.f);
        WPROPERTY(glm::vec3, color, glm::vec3(0.5, 0.5, 0.5));

    public:

    };
    
}
