#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TSparseSet.hpp"
#include "WSystems/WSystems.hpp"

#include <unordered_map>

/**
 * @brief Active in use systems.
 */
class WENGINE_API WSystemsRunner {
private:

    using Systems = std::unordered_map<WLevelId, TSparseSet<WSystemFn>>;

public:

    WSystemsRunner() noexcept =default;

    ~WSystemsRunner()=default;

    WSystemsRunner(const WSystemsRunner & other) = default;

    WSystemsRunner(WSystemsRunner && other) noexcept = default;

    WSystemsRunner & operator=(const WSystemsRunner & other) = default;

    WSystemsRunner & operator=(WSystemsRunner && other) noexcept = default;

    // Systems

    WLevelSystemId AddInitSystem(
        const WLevelId & in_level_id,
        const WSystemId & in_system_id,
        const WSystemFn & in_fn);

    WLevelSystemId AddPreSystem(
        const WLevelId & in_level_id,
        const WSystemId & in_system_id,
        const WSystemFn & in_fn);

    WLevelSystemId AddPostSystem(
        const WLevelId & in_level,
        const WSystemId & in_system_id,
        const WSystemFn & in_fn);

    WLevelSystemId AddEndSystem(
        const WLevelId & in_level,
        const WSystemId & in_system_id,
        const WSystemFn & in_fn);

    void RemoveSystem(const WLevelSystemId & in_id);

    void RemoveSystems(const WLevelId & in_level_id);

    void Clear();

    // Run Systems

    void RunInitSystems(const WLevelId & levelid, const WSystemParameters &) const;

    void RunPreSystems(const WLevelId & levelid, const WSystemParameters &) const;

    void RunPostSystems(const WLevelId & levelid, const WSystemParameters &) const;

    void RunEndSystems(const WLevelId & levelid, const WSystemParameters &) const;

private:

    enum class ESystemLocation : std::uint8_t {
        INIT,
        PRE,
        POST,
        END
    };

    WLevelSystemId AddSystem(Systems & out_system,
                             const ESystemLocation & in_location,
                             const WLevelId & in_level_id,
                             const WSystemId & in_system_id,
                             const WSystemFn & in_system);

    Systems init_systems_;
    Systems pre_systems_;
    Systems post_systems_;
    Systems end_systems_;

    std::unordered_map<WLevelSystemId, ESystemLocation> systemid_location_;
    // std::unordered_map<WSystemId, WLevelId> systemid_levelid_;

};
