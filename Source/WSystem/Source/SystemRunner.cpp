#include "WSystem/SystemRunner.hpp"
#include "WCore/WCore.hpp"


wcr::wid::WLevelSystemId wsm::SystemRunner::AddLevelSystem(
    ESystemLocation system_location,
    wcr::wid::WAssetId level_id,
    wcr::wid::WSystemId system_id,
    wsm::SystemFn system
    ) {
    return AddSystem(GetSystemContainer(system_location),
                     system_location,
                     level_id,
                     system_id,
                     system);
}

wcr::wid::WLevelSystemId wsm::SystemRunner::AddInitSystem(const wcr::wid::WAssetId & in_level_id,
                                        const wcr::wid::WSystemId & in_system_id,
                                        const wsm::SystemFn & in_system) {
    return AddSystem(init_systems_,
                     ESystemLocation::PRE,
                     in_level_id,
                     in_system_id,
                     in_system);
}

wcr::wid::WLevelSystemId wsm::SystemRunner::AddPreSystem(const wcr::wid::WAssetId & in_level_id,
                                       const wcr::wid::WSystemId & in_system_id,
                                       const wsm::SystemFn & in_system) {
    return AddSystem(pre_systems_,
                     ESystemLocation::PRE,
                     in_level_id,
                     in_system_id,
                     in_system);
}

wcr::wid::WLevelSystemId wsm::SystemRunner::AddPostSystem(const wcr::wid::WAssetId & in_level_id,
                                        const wcr::wid::WSystemId & in_system_id,
                                        const wsm::SystemFn & in_system) {
    return AddSystem(post_systems_,
                     ESystemLocation::POST,
                     in_level_id,
                     in_system_id,
                     in_system);
}

wcr::wid::WLevelSystemId wsm::SystemRunner::AddEndSystem(const wcr::wid::WAssetId & in_level_id,
                                       const wcr::wid::WSystemId & in_system_id,
                                       const wsm::SystemFn & in_system) {
    return AddSystem(end_systems_,
                     ESystemLocation::END,
                     in_level_id,
                     in_system_id,
                     in_system);
}

void wsm::SystemRunner::RemoveSystem(const wcr::wid::WLevelSystemId & in_id) {
    wcr::wid::WAssetId lvlid;
    wcr::wid::WSystemId sysid;

    in_id.ExtractWIds(lvlid, sysid);

    auto & system_container = GetSystemContainer(systemid_location_[in_id]);

    system_container[lvlid].Remove(in_id.GetId());

    systemid_location_.extract(in_id);
}

void wsm::SystemRunner::RemoveSystems(const wcr::wid::WAssetId & in_level_id) {
    init_systems_.erase(in_level_id);
    pre_systems_.erase(in_level_id);
    post_systems_.erase(in_level_id);
    end_systems_.erase(in_level_id);
}

void wsm::SystemRunner::Clear() {
    init_systems_.clear();
    pre_systems_.clear();
    post_systems_.clear();
    end_systems_.clear();

    systemid_location_.clear();
}

void wsm::SystemRunner::RunLevelSystems(
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

void wsm::SystemRunner::RunInitSystems(const wcr::wid::WAssetId & in_level_id,
                                    const wsm::SystemParameters & in_parameters) const {
    if(!init_systems_.contains(in_level_id)) {
        return;
    }

    for(auto & fn : init_systems_.at(in_level_id)) {
        fn(in_parameters);
    }
}

void wsm::SystemRunner::RunPreSystems(const wcr::wid::WAssetId & in_level_id,
                                   const wsm::SystemParameters & in_parameters) const {
    if (!pre_systems_.contains(in_level_id)) return;

    for (auto & fn : pre_systems_.at(in_level_id)) {
        fn(in_parameters);
    }
}

void wsm::SystemRunner::RunPostSystems(const wcr::wid::WAssetId & in_level_id,
                                    const wsm::SystemParameters & in_parameters) const {
    if (!post_systems_.contains(in_level_id)) return;

    for (auto & fn : post_systems_.at(in_level_id)) {
        fn(in_parameters);
    }
}

void wsm::SystemRunner::RunEndSystems(const wcr::wid::WAssetId & in_level_id,
                                   const wsm::SystemParameters & in_parameters) const {
    if (!end_systems_.contains(in_level_id)) return;

    for (auto & fn : end_systems_.at(in_level_id)) {
        fn(in_parameters);
    }
}

wcr::wid::WLevelSystemId wsm::SystemRunner::AddSystem(Systems & out_system,
                                                      ESystemLocation const & in_location,
                                                      wcr::wid::WAssetId const & in_level_id,
                                                      wcr::wid::WSystemId const & in_system_id,
                                                      wsm::SystemFn const & in_system) {

    wcr::wid::WLevelSystemId lvlsysid{in_level_id, in_system_id};
    out_system[in_level_id].Insert(in_system_id.GetId(), in_system);
    systemid_location_[lvlsysid] = in_location;

    return lvlsysid;

}



