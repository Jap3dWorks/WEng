#pragma once

#include "WCore/TSparseSet.hpp"
#include "WSystem/WSystem.hpp"

#include <unordered_map>

namespace wsm {

    enum class ESystemLocation : std::uint8_t {
        INIT,
        PRE,
        MID,
        RENDER,
        POST,
        END
    };

    class WENGINE_API SystemRunner {
        
    private:

    using Systems =
        std::unordered_map<wcr::wid::WAssetId,
                           TSparseSet<SystemFn>>;

    public:

    SystemRunner() noexcept =default;
    ~SystemRunner()=default;
    SystemRunner(SystemRunner const & other) = default;
    SystemRunner(SystemRunner && other) noexcept = default;
    SystemRunner & operator=(SystemRunner const & other) = default;
    SystemRunner & operator=(SystemRunner && other) noexcept = default;

    // Systems

    // TODO one AddSystem function
    wcr::wid::WLevelSystemId AddLevelSystem(
        ESystemLocation system_location,
        wcr::wid::WAssetId level_id,
        wcr::wid::WSystemId system_id,
        wsm::SystemFn system
        );

    wcr::wid::WLevelSystemId AddInitSystem(
        wcr::wid::WAssetId const & in_level_id,
        wcr::wid::WSystemId const & in_system_id,
        SystemFn const & in_fn);

    wcr::wid::WLevelSystemId AddPreSystem(
        wcr::wid::WAssetId const & in_level_id,
        wcr::wid::WSystemId const & in_system_id,
        SystemFn const & in_fn);

    wcr::wid::WLevelSystemId AddPostSystem(
        wcr::wid::WAssetId const & in_level,
        wcr::wid::WSystemId const & in_system_id,
        SystemFn const & in_fn);

    wcr::wid::WLevelSystemId AddRenderSystem();

    wcr::wid::WLevelSystemId AddEndSystem(
        wcr::wid::WAssetId const & in_level,
        wcr::wid::WSystemId const & in_system_id,
        SystemFn const & in_fn);

    void RemoveSystem(const wcr::wid::WLevelSystemId & in_id);

    void RemoveSystems(const wcr::wid::WAssetId & in_level_id);

    void Clear();

    // Run Systems

    void RunLevelSystems(
        ESystemLocation location,
        wcr::wid::WAssetId level_id,
        SystemParameters const & parameters
        );

    void RunInitSystems(const wcr::wid::WAssetId & levelid, const SystemParameters &) const;

    void RunPreSystems(const wcr::wid::WAssetId & levelid, const SystemParameters &) const;

    void RunPostSystems(const wcr::wid::WAssetId & levelid, const SystemParameters &) const;

    void RunEndSystems(const wcr::wid::WAssetId & levelid, const SystemParameters &) const;

    private:

    constexpr Systems & GetSystemContainer(ESystemLocation location) {
        switch(location) {
        case wsm::ESystemLocation::INIT:
            return init_systems_;
        case wsm::ESystemLocation::PRE:
            return pre_systems_;
        case wsm::ESystemLocation::MID:
            return mid_systems_;
        case wsm::ESystemLocation::RENDER:
            return render_systems_;
        case wsm::ESystemLocation::POST:
            return post_systems_;
        case wsm::ESystemLocation::END:
            return end_systems_;
        }
    }

    wcr::wid::WLevelSystemId AddSystem(Systems & out_system,
                                       const ESystemLocation & in_location,
                                       wcr::wid::WAssetId const & in_level_id,
                                       const wcr::wid::WSystemId & in_system_id,
                                       const SystemFn & in_system);

    Systems init_systems_;
    Systems pre_systems_;
    Systems mid_systems_;
    Systems render_systems_;
    Systems post_systems_;
    Systems end_systems_;

    std::unordered_map<wcr::wid::WLevelSystemId, ESystemLocation> systemid_location_;

    };

}
