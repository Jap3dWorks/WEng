#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TFunction.hpp"
#include "WEngineObjects/TWRef.hpp"
#include "WEngineObjects/WActor.hpp"
#include "WEngineObjects/WComponent.hpp"
#include "WEngineObjects/WEngineCycleData.hpp"

// TODO Serializable

class WClass;

class  ILevel {

public:

    virtual ~ILevel()=default;

    /**
     * @brief Call this method before Level starts.
     */
    virtual void Init();

    /**
     * @brief Create WActor derived objects.
     */
    virtual WId CreateActor(const WClass * in_class)=0;

    /**
     * @brief Get an Actor reference from WId.
     */
    virtual TWRef<WActor> GetActor(const WId & in_id) const =0;

    virtual void ForEachActor(const WClass * in_class,
                              TFunction<void(WActor*)> in_predicate) const=0;

    virtual WId CreateComponent(const WId & in_actor,
                                const WClass * in_class)=0;

    virtual TWRef<WComponent> GetComponent(const WClass * in_class,
                                        const WId & in_component_id) const=0;

    /**
     * @brief Call each cycle.
     */
    virtual void Update(const WEngineCycleData & in_cycle_data)=0;

    /**
     * @breif Close the resources in Use.
     * This is method should be called just before the level use ends.
     */
    virtual void Close()=0;

    virtual std::string Name() const=0;

    virtual WId WID()=0;

    virtual void WID(const WId & in_id)=0;
};

