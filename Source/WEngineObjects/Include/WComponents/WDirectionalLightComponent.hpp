#pragma once

#include "WEngineObjects/WComponent.hpp"
#include "WEngineObjects/WObjectMacros.hpp"

#include <glm/glm.hpp>

#include "WDirectionalLightComponent.WEngine.hpp"

class WENGINEOBJECTS_API WDirectionalLightComponent : public WComponent {
    WOBJECT_BODY(WDirectionalLightComponent);

public:

    bool Active() const noexcept {
        return active_;
    }

    void Active(const bool & in_active) noexcept {
        active_ = in_active;
    }

    float Intensity() const noexcept {
        return intensity_;
    }

    void Intensity(const float & in_intensity) noexcept {
        intensity_ = in_intensity;
    }

    glm::vec4 Color() const noexcept {
        return color_;
    }

    void Color(const glm::vec4 & in_color) noexcept {
        color_ = in_color;
    }

private:

    bool active_{true};
    float intensity_{1.f};
    glm::vec4 color_{0.5, 0.5, 0.5, 1.f};

};
