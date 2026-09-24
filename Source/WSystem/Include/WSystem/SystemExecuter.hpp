#pragma once

#include "WCore/TSparseSet.hpp"
#include "WSystem/WSystem.hpp"

#include <unordered_map>

namespace wsm {

    enum class ESystemLocation : std::uint8_t {
        INIT,
        PRE,
        MID,
        POST,  // after draw scene
        END
    };

    class WENGINE_API SystemExecuter {
        
    private:

    using SystemsContainer =
        std::unordered_map<wcr::wid::WAssetId,
                           TSparseSet<SystemFn>>;

    public:

    SystemExecuter() noexcept =default;
    ~SystemExecuter()=default;
    SystemExecuter(SystemExecuter const & other) = default;
    SystemExecuter(SystemExecuter && other) noexcept = default;
    SystemExecuter & operator=(SystemExecuter const & other) = default;
    SystemExecuter & operator=(SystemExecuter && other) noexcept = default;

    wcr::wid::WLevelSystemId AddLevelSystem(
        ESystemLocation system_location,
        wcr::wid::WAssetId level_id,
        wcr::wid::WSystemId system_id,
        wsm::SystemFn system
        );

    void RemoveSystem(const wcr::wid::WLevelSystemId & in_id);

    void RemoveSystems(const wcr::wid::WAssetId & in_level_id);

    void Clear();

    // Run Systems

    void RunLevelSystems(
        ESystemLocation location,
        wcr::wid::WAssetId level_id,
        SystemParameters const & parameters
        );

    private:

    constexpr SystemsContainer & GetSystemContainer(ESystemLocation location) {
        switch(location) {
        case wsm::ESystemLocation::INIT:
            return init_systems_;
        case wsm::ESystemLocation::PRE:
            return pre_systems_;
        case wsm::ESystemLocation::MID:
            return mid_systems_;
        case wsm::ESystemLocation::POST:
            return post_systems_;
        case wsm::ESystemLocation::END:
            return end_systems_;
        }
    }

    SystemsContainer init_systems_;
    SystemsContainer pre_systems_;
    SystemsContainer mid_systems_;
    SystemsContainer post_systems_;
    SystemsContainer end_systems_;

    std::unordered_map<wcr::wid::WLevelSystemId, ESystemLocation> systemid_location_;

    };

}
