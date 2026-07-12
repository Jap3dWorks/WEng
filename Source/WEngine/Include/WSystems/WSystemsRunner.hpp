#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TSparseSet.hpp"
#include "WSystems/WSystems.hpp"

#include <unordered_map>
#include <variant>

/**
 * @brief Active in use systems.
 */
class WENGINE_API WSystemsRunner {
private:

    using Systems = std::unordered_map<wid::WAssetId, TSparseSet<WSystemFn>>;

public:

    WSystemsRunner() noexcept =default;

    ~WSystemsRunner()=default;

    WSystemsRunner(const WSystemsRunner & other) = default;

    WSystemsRunner(WSystemsRunner && other) noexcept = default;

    WSystemsRunner & operator=(const WSystemsRunner & other) = default;

    WSystemsRunner & operator=(WSystemsRunner && other) noexcept = default;

    // Systems

    wid::WLevelSystemId AddInitSystem(
        wid::WAssetId const & in_level_id,
        wid::WSystemId const & in_system_id,
        WSystemFn const & in_fn);

    wid::WLevelSystemId AddPreSystem(
        wid::WAssetId const & in_level_id,
        wid::WSystemId const & in_system_id,
        WSystemFn const & in_fn);

    wid::WLevelSystemId AddPostSystem(
        wid::WAssetId const & in_level,
        wid::WSystemId const & in_system_id,
        WSystemFn const & in_fn);

    wid::WLevelSystemId AddEndSystem(
        wid::WAssetId const & in_level,
        wid::WSystemId const & in_system_id,
        WSystemFn const & in_fn);
    

    void RemoveSystem(const wid::WLevelSystemId & in_id);

    void RemoveSystems(const wid::WAssetId & in_level_id);

    void Clear();

    // Run Systems

    void RunInitSystems(const wid::WAssetId & levelid, const WSystemParameters &) const;

    void RunPreSystems(const wid::WAssetId & levelid, const WSystemParameters &) const;

    void RunPostSystems(const wid::WAssetId & levelid, const WSystemParameters &) const;

    void RunEndSystems(const wid::WAssetId & levelid, const WSystemParameters &) const;

private:

    enum class ESystemLocation : std::uint8_t {
        INIT,
        PRE,
        POST,
        END
    };

    wid::WLevelSystemId AddSystem(Systems & out_system,
                             const ESystemLocation & in_location,
                             wid::WAssetId const & in_level_id,
                             const wid::WSystemId & in_system_id,
                             const WSystemFn & in_system);

    Systems init_systems_;
    Systems pre_systems_;
    Systems post_systems_;
    Systems end_systems_;

    std::unordered_map<wid::WLevelSystemId, ESystemLocation> systemid_location_;

};
