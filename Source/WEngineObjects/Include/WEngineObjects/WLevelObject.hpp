#pragma once

#include "WEngineObjects/WObject.hpp"
#include "WEngineObjects/WEngineCycleData.hpp"

#include "WLevelObject.WEngine.hpp"


class ILevel;

/**
 * @brief Base class for objects that can be present in a level (WActors and WComponents).
*/
WCLASS()
class WCORE_API WLevelObject : public WObject
{

    WOBJECT_BODY(WLevelObject)

public:

    virtual void OnUpdate(const WEngineCycleData & in_cycle_data) {}

    virtual void OnInit() {}

    virtual void OnClose() {}

    ILevel * Level() const { return level; }

    void Level(ILevel * in_level) { level = in_level; }

private:

    ILevel * level;

};

