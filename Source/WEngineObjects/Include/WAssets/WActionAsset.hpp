#pragma once

#include "WCore/WCore.hpp"
#include "WStructs/WEngineStructs.hpp"
#include "WEngineObjects/WAsset.hpp"

#include "WActionAsset.WEngine.hpp"

/* WActions to be triggered as input system. */
WCLASS()
class WENGINEOBJECTS_API WActionAsset : public WAsset {

    WOBJECT_BODY(WActionAsset)

    WActionStruct & ActionStruct() {
        return action_struct_;
    }

public:

    WActionStruct action_struct_{};

};
