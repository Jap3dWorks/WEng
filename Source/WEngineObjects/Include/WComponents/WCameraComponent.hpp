#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WComponent.hpp"
// #include "WStructs/WLinearAlgebraStructs.hpp"

#include "WCameraComponent.WEngine.hpp"


class WENGINEOBJECTS_API WCameraComponent : public WComponent {
    WOBJECT_BODY(WCameraComponent)
        
public:

    // void Transform(const WTransformStruct & in_transform) {
    //     transform_ = in_transform;
    // }

    // WTransformStruct & Transform() { return transform_; }

    // const WTransformStruct & Transform() const { return transform_; }

private:
    
    // Transform data
    // WTransformStruct transform_{};
  
};
