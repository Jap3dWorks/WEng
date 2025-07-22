#pragma once

#include "WEngineObjects/WComponent.hpp"
#include "WStructs/WLinearAlgebraStructs.hpp"

#include "WTransformComponent.WEngine.hpp"


class WENGINEOBJECTS_API WTransformComponent : WComponent {
    WOBJECT_BODY(WTransformComponent)
        
public:

    void Transform(const WTransformStruct & in_transform) {
        transform_ = in_transform;
    }

    WTransformStruct & Transform() { return transform_; }

    const WTransformStruct & Transform() const { return transform_; }

private:
    
    // Transform data
    WTransformStruct transform_{};
  
};
