#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WComponent.hpp"
#include "WStructs/WComponentStructs.hpp"

#include <glm/glm.hpp>

#include "WCameraComponent.WEngine.hpp"


class WENGINEOBJECTS_API WCameraComponent : public WComponent {
    WOBJECT_BODY(WCameraComponent)
        
public:

    WCameraStruct & CameraStruct() { return camera_struct_; };

    const WCameraStruct & CameraStruct() const { return camera_struct_; }

private:

    WCameraStruct camera_struct_;
    
};
