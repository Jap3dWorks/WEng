#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TFunction.hpp"
#include "WEngineObjects/TWRef.hpp"
#include "WEngineObjects/WEngineCycleData.hpp"

#include <memory>

// TODO Serializable

class WClass;
class WActor;
class WComponent;

class  ILevel {

public:

    virtual ~ILevel()=default;

    virtual std::unique_ptr<ILevel> Clone() const =0;

    /**
     * @brief Call this method before Level starts.
     */
    virtual void Init(const WEngineCycleData & in_cycle_data)=0;

    /**
     * @brief Call each cycle.
     */
    virtual void Update(const WEngineCycleData & in_cycle_data)=0;

    /**
     * @breif Close the resources in Use.
     * This is method should be called just before the level use ends.
     */
    virtual void Close(const WEngineCycleData & in_cycle_data)=0;

    /**
     * @brief Create WActor derived objects.
     */
    virtual WId CreateActor(const WClass * in_class)=0;

    /**
     * @brief Get an Actor reference from WId.
     */
    virtual TWRef<WActor> GetActor(const WId & in_id) =0;

    /**
     * @brief Iter over all actors of class in_class and derived classes.
     * So WActor should iterate over all level actors.
     */
    virtual void ForEachActor(const WClass * in_class,
                              TFunction<void(WActor*)> in_predicate) const=0;

    virtual WId CreateComponent(const WId & in_actor,
                                const WClass * in_class)=0;

    /**
     * @brief Get component of class in_class and id in_component_id.
     */
    virtual TWRef<WComponent> GetComponent(const WClass * in_class,
                                           const WId & in_component_id)=0;

    // virtual void ForEachComponent(const WId & in_actor_id,
    //                               TFunction<void(WComponent*)> in_component)=0;

    virtual void ForEachComponent(const WClass * in_component_class,
                                  TFunction<void(WComponent*)> in_predicate)=0;

    virtual std::string Name() const=0;

    virtual WId WID() const=0;

    virtual void WID(const WId & in_id)=0;
};

