#pragma once

#include "WCore/WCore.hpp"
#include "WObjects/WObject.hpp"

#include "WComponent.WEng.hpp"

class WCORE_API WComponent : public WObject
{

public:

    WOBJECT_BODY

public:

    WPROPERTY(wcr::wid::WEntityId, entity_id, 0);

private:

};

