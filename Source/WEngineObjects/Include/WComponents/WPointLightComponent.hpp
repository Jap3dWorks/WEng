#pragma once

#include "WEngineObjects/WComponent.hpp"

#include <glm/glm.hpp>

class WENGINEOBJECTS_API WPointLightComponent : public WComponent {
public:

private:
    bool active{true};
    float intensity{1.f};
    glm::vec4 color{0.5, 0.5, 0.5, 1.f};
    float radius{10.f};
};
