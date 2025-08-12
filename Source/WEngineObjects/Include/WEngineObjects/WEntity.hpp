#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WObjectMacros.hpp"
#include "WStructs/WEngineStructs.hpp"
#include "WEngineObjects/WObject.hpp"

#include <cstring>

#include "WEntity.WEngine.hpp"

/**
 * @brief Base class for all actors in the engine. 
 * Entities are composed by components in the level.
*/
WCLASS()
class WCORE_API WEntity : public WObject
{

    WOBJECT_BODY(WEntity)

public:

    constexpr WEntityId WID() const noexcept {
        return wid_;
    }

    constexpr void WID(const WEntityId & in_id) noexcept {
        assert(wid_.GetId() == 0);
        wid_ = in_id;
    }

    const char * Name() const {
        return name_;
    }

    void Name(const char * in_name) {
        std::strcpy(name_, in_name);
    }

    virtual void OnUpdate(const WEngineCycleStruct & in_cycle_data) {}

    virtual void OnInit(const WEngineCycleStruct & in_cycle_data) {}

    virtual void OnClose(const WEngineCycleStruct & in_cycle_data) {}

private:

    char name_[WOBJECT_NAME_SIZE];

    WEntityId wid_;

};

