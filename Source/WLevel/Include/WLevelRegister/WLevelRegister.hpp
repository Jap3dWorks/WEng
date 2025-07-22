#pragma once

#include "WCore/WCore.hpp"
#include "WEngineInterfaces/ILevelRegister.hpp"
#include "WCore/WIdPool.hpp"
#include "WCore/TFunction.hpp"

#include <unordered_map>
#include <memory>

class ILevel;

/**
 * @brief Register Open and Close Levels.
 */
class WLEVEL_API WLevelRegister: public ILevelRegister {
public:

    WLevelRegister() = default;

    virtual ~WLevelRegister() = default;

    WId RegisterLevel(std::unique_ptr<ILevel> && in_level) override;

    bool LoadLevel(const WId & in_id) override;

    WId Current() const override;

    TOptionalRef<ILevel> Get(const WId & in_id) override;

    void ApplyCurrent(const TFunction<void(ILevel*)> & fn) const override;

    void Apply(const WId & in_id, const TFunction<void(ILevel*)> & fn) const override;

private:
    
    WIdPool id_pool_;

    WId current_;

    std::unordered_map<WId, std::unique_ptr<ILevel>> levels_;

};

