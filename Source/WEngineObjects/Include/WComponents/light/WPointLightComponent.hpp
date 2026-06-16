#pragma once

#include "WEngineObjects/WComponent.hpp"
#include "WEngineObjects/WObjectMacros.hpp"

#include <glm/glm.hpp>

#include "WPointLightComponent.WEngine.hpp"

class WENGINEOBJECTS_API WPointLightComponent : public WComponent {

    WOBJECT_BODY(WPointLightComponent)

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

    float Radius() const noexcept {
        return radius_;
    }

    void Radius(const float & in_radius) noexcept {
        radius_ = in_radius;
    }

private:

    bool active_{true};
    float intensity_{1.f};
    glm::vec4 color_{0.5, 0.5, 0.5, 1.f};
    float radius_{10.f};
};
