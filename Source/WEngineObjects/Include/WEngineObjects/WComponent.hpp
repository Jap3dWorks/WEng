#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WObject.hpp"

#include "WComponent.WEngine.hpp"

class WCORE_API WComponent : public WObject
{

public:

    WOBJECT_BODY

public:

    WPROPERTY(wcr::wid::WEntityId, entity_id, 0);

private:

};

