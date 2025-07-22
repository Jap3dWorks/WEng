#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TFunction.hpp"
#include "WCore/TOptionalRef.hpp"

#include <memory>

class ILevel;

/**
 * @brief Create and manage Levels.
 */
class ILevelRegister {
public:

    virtual ~ILevelRegister() = default;

    /**
     * Add a new Level into the register and assign an unique WId.
     */
    virtual WId RegisterLevel(std::unique_ptr<ILevel> && in_level)=0;

    /**
     * Load the registered level with WId in_id. This level is the new current level.
     */
    virtual bool LoadLevel(const WId & in_id)=0;

    /**
     * Returns current level WId.
     */
    virtual WId Current() const=0;

    virtual TOptionalRef<ILevel> Get(const WId & in_id)=0;

    /**
     * Apply in_fn over the current level.
     */
    virtual void ApplyCurrent(const TFunction<void(ILevel*)> & in_fn) const=0;

    /**
     * Apply in_fn over the level with WId in_id
     */
    virtual void Apply(const WId & in_id, const TFunction<void(ILevel*)> & in_fn) const=0;

};

