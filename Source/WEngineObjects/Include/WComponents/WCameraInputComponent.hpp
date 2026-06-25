#pragma once

#include "WCore/WCore.hpp"

#include "WEngineObjects/WComponent.hpp"

#include "WCameraInputComponent.WEngine.hpp"

class WENGINEOBJECTS_API WCameraInputComponent : public WComponent {

    WOBJECT_BODY

public:

    WPROPERTY(bool, front, false);
    WPROPERTY(bool, back, false);
    WPROPERTY(bool, left, false);
    WPROPERTY(bool, right, false);
    
};

