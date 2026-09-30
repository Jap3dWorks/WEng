#pragma once

#include "WObjects/WComponent.hpp"
#include "WMath/Geometry/Shape.hpp"
#include "WCollision/CollisionTree.hpp"
#include "WCollision/CollisionTrack.hpp"

#include <variant>

#include "wcl::component::CollisionLevelData.WEng.hpp"

namespace wcl::component {

    class WCOLLISION_API CollisionLevelData : public WComponent {

        WOBJECT_BODY;

    public:

        WPROPERTY(wcl::CollisionTree, tree, );
        WPROPERTY(wcl::CollisionTrack, track, )
        

    };
}
