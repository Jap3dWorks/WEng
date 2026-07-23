#pragma once

#include "WCore/WCore.hpp"

#include "WEngineObjects/WComponent.hpp"

#include "wcm::CameraInput.WEng.hpp"

namespace wcm {

    class WENGINEOBJECTS_API CameraInput : public WComponent {

        WOBJECT_BODY;

    public:

        WPROPERTY(bool, front, false);
        WPROPERTY(bool, back, false);
        WPROPERTY(bool, left, false);
        WPROPERTY(bool, right, false);
    
    };

}
