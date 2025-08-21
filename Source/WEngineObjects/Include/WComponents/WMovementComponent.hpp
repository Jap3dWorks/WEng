#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WComponent.hpp"

#include <glm/glm.hpp>

#include "WMovementComponent.WEngine.hpp"

class WENGINEOBJECTS_API WMovementComponent : public WComponent {
    WOBJECT_BODY(WMovementComponent)
        
public:

    glm::vec3 Velocity() const noexcept {
        return velocity_;
    }

    void Velocity(const glm::vec3 & in_velocity_direction) noexcept {
        velocity_ = in_velocity_direction;
    }

    float MaxVelocity() const noexcept {
        return max_velocity_;
    }

    void MaxVelocity(const float & in_velocity)  noexcept {
        max_velocity_ = in_velocity;
    }

    glm::vec3 Acceleration() const noexcept { return acceleration_; }

    void Acceleration(const glm::vec3 & in_direction)  noexcept {
        acceleration_ = in_direction;
    }

    float MaxAcceleration() const noexcept { return max_acceleration_; }

    void MaxAcceleration(const float & in_max_acceleration) noexcept {
        max_acceleration_ = in_max_acceleration;
    }

    float Drag() const noexcept { return drag_; }

    void Drag(const float & in_value) noexcept { drag_ = in_value; }

private:

    glm::vec3 velocity_{0};
    float max_velocity_{30.f};

    glm::vec3 acceleration_{0};
    float max_acceleration_{30.f};

    float drag_{2.5f};

};
