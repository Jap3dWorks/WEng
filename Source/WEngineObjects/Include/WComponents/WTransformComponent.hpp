#pragma once

#include "WEngineObjects/WComponent.hpp"
#include "WComponents/WComponentTypes.hpp"

#include "WCoreTypes/WMathStructs.hpp"

#include "WTransformComponent.WEngine.hpp"


class WENGINEOBJECTS_API WTransformComponent : public WComponent {

    WOBJECT_BODY
        
public:

    WPROPERTY(glm::vec3, position, 0.0);
    WPROPERTY(glm::vec3, rotation, 0.0);
    WPROPERTY(glm::vec3, scale, 1.0);
    WPROPERTY(ERotationOrder, rotation_order, ERotationOrder::zxy);
    WPROPERTY(glm::mat4, transform_matrix, 1.0);
    
public:

private:
  
};
