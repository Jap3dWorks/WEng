#pragma once

#include "WEngineObjects/WComponent.hpp"
#include "WStructs/WComponentStructs.hpp"

#include "WTransformComponent.WEngine.hpp"


class WENGINEOBJECTS_API WTransformComponent : public WComponent {
    WOBJECT_BODY(WTransformComponent)
        
public:

    void TransformStruct(const WTransformStruct & in_transform) {
        transform_ = in_transform;
    }

    WTransformStruct & TransformStruct() { return transform_; }

    const WTransformStruct & TransformStruct() const { return transform_; }

private:
    
    // Transform data
    WTransformStruct transform_{};
  
};
