#pragma once

#include "WObjects/WComponent.hpp"
#include "WCoreTypes/WRenderTypes.hpp"

#include "WCoreTypes/WMathStructs.hpp"
#include "WUtils/WMath.hpp"

#include "wcm::Transform.WEng.hpp"

namespace wcm {
    
    class WOBJECTS_API Transform : public WComponent {

        WOBJECT_BODY
        
            public:

        WPROPERTY(glm::vec3, position, 0.0);
        WPROPERTY(glm::vec3, rotation, 0.0);
        WPROPERTY(glm::vec3, scale, 1.0);
        WPROPERTY(ERotationOrder, rotation_order, ERotationOrder::zxy);
        WPROPERTY(glm::mat4, transform_matrix, 1.0);
    
    public:

        void SetTransformMatrix(glm::mat4 in_transform_matrix) {

            auto [position, rotation, scale]  = WMath::ToPositionRotationScale(
                in_transform_matrix, rotation_order);

            Set_position(std::move(position));
            Set_rotation(std::move(rotation));
            Set_scale(std::move(scale));

            Set_transform_matrix(std::move(in_transform_matrix));
        }

    private:
  
    };

}
