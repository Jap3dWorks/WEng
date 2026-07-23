#pragma once

#include "WCore/WCore.hpp"
#include "WCoreTypes/WCoreTypes.hpp"
#include "WCoreTypes/WEngineStructs.hpp"
#include "WEngineObjects/WAsset.hpp"

#include "was::Action.WEng.hpp"

namespace was {

    using ActionName = TName<32>;

    /** WActions to be triggered as input system. */
    class WENGINEOBJECTS_API Action : public WAsset {

        WOBJECT_BODY;

    public:

        WPROPERTY(ActionName, action_name, );

    public:

    };
    
}
