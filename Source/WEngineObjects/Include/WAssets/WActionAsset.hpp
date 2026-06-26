#pragma once

#include "WCore/WCore.hpp"
#include "WCoreTypes/WCoreTypes.hpp"
#include "WCoreTypes/WEngineStructs.hpp"
#include "WEngineObjects/WAsset.hpp"

#include "WActionAsset.WEngine.hpp"

using WActionName = TName<32>;

/** WActions to be triggered as input system. */
class WENGINEOBJECTS_API WActionAsset : public WAsset {

    WOBJECT_BODY;

public:

    WPROPERTY(WActionName, action_name, );

public:

};
