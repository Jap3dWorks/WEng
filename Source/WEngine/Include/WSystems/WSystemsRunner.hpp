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

    using Systems = std::unordered_map<wcr::wid::WAssetId, TSparseSet<WSystemFn>>;

public:

    WSystemsRunner() noexcept =default;

    ~WSystemsRunner()=default;

    WSystemsRunner(const WSystemsRunner & other) = default;

    WSystemsRunner(WSystemsRunner && other) noexcept = default;

    WSystemsRunner & operator=(const WSystemsRunner & other) = default;

    WSystemsRunner & operator=(WSystemsRunner && other) noexcept = default;

    // Systems

    wcr::wid::WLevelSystemId AddInitSystem(
        wcr::wid::WAssetId const & in_level_id,
        wcr::wid::WSystemId const & in_system_id,
        WSystemFn const & in_fn);

    wcr::wid::WLevelSystemId AddPreSystem(
        wcr::wid::WAssetId const & in_level_id,
        wcr::wid::WSystemId const & in_system_id,
        WSystemFn const & in_fn);

    wcr::wid::WLevelSystemId AddPostSystem(
        wcr::wid::WAssetId const & in_level,
        wcr::wid::WSystemId const & in_system_id,
        WSystemFn const & in_fn);

    wcr::wid::WLevelSystemId AddEndSystem(
        wcr::wid::WAssetId const & in_level,
        wcr::wid::WSystemId const & in_system_id,
        WSystemFn const & in_fn);
    

    void RemoveSystem(const wcr::wid::WLevelSystemId & in_id);

    void RemoveSystems(const wcr::wid::WAssetId & in_level_id);

    void Clear();

    // Run Systems

    void RunInitSystems(const wcr::wid::WAssetId & levelid, const WSystemParameters &) const;

    void RunPreSystems(const wcr::wid::WAssetId & levelid, const WSystemParameters &) const;

    void RunPostSystems(const wcr::wid::WAssetId & levelid, const WSystemParameters &) const;

    void RunEndSystems(const wcr::wid::WAssetId & levelid, const WSystemParameters &) const;

private:

    enum class ESystemLocation : std::uint8_t {
        INIT,
        PRE,
        POST,
        END
    };

    wcr::wid::WLevelSystemId AddSystem(Systems & out_system,
                             const ESystemLocation & in_location,
                             wcr::wid::WAssetId const & in_level_id,
                             const wcr::wid::WSystemId & in_system_id,
                             const WSystemFn & in_system);

    Systems init_systems_;
    Systems pre_systems_;
    Systems post_systems_;
    Systems end_systems_;

    std::unordered_map<wcr::wid::WLevelSystemId, ESystemLocation> systemid_location_;

};
