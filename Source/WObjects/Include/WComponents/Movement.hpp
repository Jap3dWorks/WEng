#pragma once

#include "WCore/WCore.hpp"
#include "WObjects/WComponent.hpp"

#include <glm/glm.hpp>

#include "wcm::Movement.WEng.hpp"

namespace wcm {

    class WOBJECTS_API Movement : public WComponent {

        WOBJECT_BODY;

    public:        

        WPROPERTY(glm::vec3, velocity, 0);
        WPROPERTY(float, max_velocity, 30.f);
        WPROPERTY(glm::vec3, acceleration, 0);
        WPROPERTY(float, max_acceleration, 30.f);
        WPROPERTY(float, drag, 2.5f);
    };

}
