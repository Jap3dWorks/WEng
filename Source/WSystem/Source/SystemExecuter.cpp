#include "WSystem/SystemExecuter.hpp"
#include "WCore/WCore.hpp"

wcr::wid::WLevelSystemId wsm::SystemExecuter::AddLevelSystem(
    ESystemLocation system_location,
    wcr::wid::WAssetId level_id,
    wcr::wid::WSystemId system_id,
    wsm::SystemFn system
    ) {
    auto & system_container = GetSystemContainer(system_location);

    wcr::wid::WLevelSystemId lvlsysid{level_id, system_id};
    system_container[level_id].Insert(system_id.GetId(), system);
    systemid_location_[lvlsysid] = system_location;

    return lvlsysid;
}

void wsm::SystemExecuter::RemoveSystem(const wcr::wid::WLevelSystemId & in_id) {
    wcr::wid::WAssetId lvlid;
    wcr::wid::WSystemId sysid;

    in_id.ExtractWIds(lvlid, sysid);

    auto & system_container = GetSystemContainer(systemid_location_[in_id]);

    system_container[lvlid].Remove(in_id.GetId());

    systemid_location_.extract(in_id);
}

void wsm::SystemExecuter::RemoveSystems(const wcr::wid::WAssetId & in_level_id) {
    init_systems_.erase(in_level_id);
    pre_systems_.erase(in_level_id);
    post_systems_.erase(in_level_id);
    end_systems_.erase(in_level_id);
}

void wsm::SystemExecuter::Clear() {
    init_systems_.clear();
    pre_systems_.clear();
    post_systems_.clear();
    end_systems_.clear();

    systemid_location_.clear();
}

void wsm::SystemExecuter::RunLevelSystems(
    wsm::ESystemLocation location,
    wcr::wid::WAssetId level_id,
    SystemParameters const & parameters
    ) {

    auto & system_container = GetSystemContainer(location);

    if(!system_container.contains(level_id)) {
        return;
    }

    for(auto & fn : system_container.at(level_id)) {
        fn(parameters);
    }   
}


