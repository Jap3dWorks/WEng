#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WObject.hpp"

#include "WComponent.WEngine.hpp"

WCLASS()
class WCORE_API WComponent : public WObject
{

public:

    WOBJECT_BODY(WComponent)

public:

    WEntityId EntityId() const {
        return entity_id_;
    }

    void EntityId(const WEntityId & in_id) {
        entity_id_ = in_id;
    }

private:

    WEntityId entity_id_;

};

