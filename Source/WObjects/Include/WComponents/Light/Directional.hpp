#pragma once

#include "WCoreTypes/WRenderTypes.hpp"
#include "WObjects/WComponent.hpp"

#include <glm/glm.hpp>

#include "wcm::light::Directional.WEng.hpp"

namespace wcm::light {

    // TODO use this enum to codify lights binary states.
    enum class State : std::uint8_t {
        ACTIVE=0b1,
        CAST_SHADOWS=0b10
    };

    class WOBJECTS_API Directional : public WComponent {

        WOBJECT_BODY;

        WPROPERTY(bool, active, true);
        WPROPERTY(bool, cast_shadows, false);
        WPROPERTY(float, intensity, 1.f);
        WPROPERTY(glm::vec3, color, glm::vec3(0.5, 0.5, 0.5));

    public:

    };
    
}
