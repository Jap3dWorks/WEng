#pragma once

#include "WCore/WCore.hpp"

#include "WEngineObjects/WComponent.hpp"

#include "WCameraInputComponent.WEngine.hpp"

class WENGINEOBJECTS_API WCameraInputComponent : public WComponent {
    WOBJECT_BODY(WCameraInputComponent)

public:

    bool Front() const noexcept { return front_; }
    void Front(bool in_value) noexcept { front_ = in_value; }
    bool Back() const noexcept { return back_; }
    void Back(bool in_value) noexcept { back_ = in_value; }
    bool Left() const noexcept { return left_; }
    void Left(bool in_value) noexcept { left_ = in_value; }
    bool Right() const noexcept { return right_; }
    void Right(bool in_value) noexcept { right_ = in_value;}

private:
        
    bool front_{false};
    bool back_{false};
    bool left_{false};
    bool right_{false};
};

