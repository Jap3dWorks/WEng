#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WIdPool.hpp"
#include "WCore/TSparseSet.hpp"
#include "WSystems/WSystems.hpp"

#include <unordered_map>

/**
 * @brief Active in use systems.
 */
class WENGINE_API WSystemDb {
private:

    using Systems = TSparseSet<WSystems::WSystemFn>;
    using LevelSystems = std::unordered_map<WLevelId, TSparseSet<WSystems::WLevelSystemFn>>;

public:

    WSystemDb() noexcept =default;

    ~WSystemDb()=default;

    WSystemDb(const WSystemDb & other) = default;

    WSystemDb(WSystemDb && other) noexcept = default;

    WSystemDb & operator=(const WSystemDb & other) = default;

    WSystemDb & operator=(WSystemDb && other) noexcept = default;

    // Systems

    WSystemId AddInitSystem(const WSystems::WSystemFn & in_fn);

    WSystemId AddPreSystem(const WSystems::WSystemFn & in_fn);

    WSystemId AddPostSystem(const WSystems::WSystemFn & in_fn);

    WSystemId AddEndSystem(const WSystems::WSystemFn & in_fn);

    // Level Systems

    WSystemId AddInitLevelSystem(const WLevelId & in_level_id, const WSystems::WLevelSystemFn & in_fn);

    WSystemId AddPreLevelSystem(const WLevelId & in_level_id, const WSystems::WLevelSystemFn & in_fn);

    WSystemId AddPostLevelSystem(const WLevelId & in_level_id, const WSystems::WLevelSystemFn & in_fn);

    WSystemId AddEndLevelSystem(const WLevelId & in_level_id, const WSystems::WLevelSystemFn & in_fb);


    void RemoveSystem(const WSystemId & in_id);

    void Clear();

    // Run Systems

    void RunInitSystems(WEngine * in_engine) const;

    void RunPreSystems(WEngine * in_engine) const;

    void RunPostSystems(WEngine * in_engine) const;

    void RunEndSystems(WEngine * in_engine) const;

    void RunInitLevelSystems(WLevel * in_level, WEngine * in_engine) const;

    void RunPreLevelSystems(WLevel * in_level, WEngine * in_engine) const;

    void RunPostLevelSystems(WLevel * in_level, WEngine * in_engine) const;

    void RunEndLevelSystems(WLevel * in_level, WEngine * in_engine) const;

private:

    enum class ESystemLocation : std::uint8_t {
        INIT,
        PRE,
        POST,
        END,
        LEVEL_INIT,
        LEVEL_PRE,
        LEVEL_POST,
        LEVEL_END
    };

    WSystemId AddSystem(Systems & out_system,
                        const ESystemLocation & in_location,
                        const WSystems::WSystemFn & in_system);

    WSystemId AddLevelSystem(LevelSystems & out_system,
                             const ESystemLocation & in_location,
                             const WLevelId & in_level_id,
                             const WSystems::WLevelSystemFn & in_system);

    Systems init_systems_;
    Systems pre_systems_;
    Systems post_systems_;
    Systems end_systems_;

    LevelSystems init_level_systems_;
    LevelSystems pre_level_systems_;
    LevelSystems post_level_systems_;
    LevelSystems end_level_systems_;

    WIdPool<WSystemId> id_pool_;

    std::unordered_map<WSystemId, ESystemLocation> systemid_location_;
    std::unordered_map<WSystemId, WLevelId> systemid_levelid_;

};
