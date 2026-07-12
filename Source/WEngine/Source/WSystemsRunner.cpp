#include "WSystems/WSystemsRunner.hpp"
#include "WCore/WCore.hpp"

wid::WLevelSystemId WSystemsRunner::AddInitSystem(const wid::WAssetId & in_level_id,
                                        const wid::WSystemId & in_system_id,
                                        const WSystemFn & in_system) {
    return AddSystem(init_systems_,
                     ESystemLocation::PRE,
                     in_level_id,
                     in_system_id,
                     in_system);
}

wid::WLevelSystemId WSystemsRunner::AddPreSystem(const wid::WAssetId & in_level_id,
                                       const wid::WSystemId & in_system_id,
                                       const WSystemFn & in_system) {
    return AddSystem(pre_systems_,
                     ESystemLocation::PRE,
                     in_level_id,
                     in_system_id,
                     in_system);
}

wid::WLevelSystemId WSystemsRunner::AddPostSystem(const wid::WAssetId & in_level_id,
                                        const wid::WSystemId & in_system_id,
                                        const WSystemFn & in_system) {
    return AddSystem(post_systems_,
                     ESystemLocation::POST,
                     in_level_id,
                     in_system_id,
                     in_system);
}

wid::WLevelSystemId WSystemsRunner::AddEndSystem(const wid::WAssetId & in_level_id,
                                       const wid::WSystemId & in_system_id,
                                       const WSystemFn & in_system) {
    return AddSystem(end_systems_,
                     ESystemLocation::END,
                     in_level_id,
                     in_system_id,
                     in_system);
}

void WSystemsRunner::RemoveSystem(const wid::WLevelSystemId & in_id) {
    wid::WAssetId lvlid;
    wid::WSystemId sysid;

    in_id.ExtractWIds(lvlid, sysid);
    
    switch(systemid_location_[in_id]) {
    case ESystemLocation::INIT:
        init_systems_[lvlid].Remove(in_id.GetId());
        break;
    case ESystemLocation::PRE:
        pre_systems_[lvlid].Remove(in_id.GetId());
        break;
    case ESystemLocation::POST:
        post_systems_[lvlid].Remove(in_id.GetId());
        break;
    case ESystemLocation::END:
        end_systems_[lvlid].Remove(in_id.GetId());
        break;
    }

    systemid_location_.extract(in_id);
}

void WSystemsRunner::RemoveSystems(const wid::WAssetId & in_level_id) {
    init_systems_.erase(in_level_id);
    pre_systems_.erase(in_level_id);
    post_systems_.erase(in_level_id);
    end_systems_.erase(in_level_id);
}

void WSystemsRunner::Clear() {
    init_systems_.clear();
    pre_systems_.clear();
    post_systems_.clear();
    end_systems_.clear();

    systemid_location_.clear();
}

void WSystemsRunner::RunInitSystems(const wid::WAssetId & in_level_id,
                                    const WSystemParameters & in_parameters) const {
    if(!init_systems_.contains(in_level_id)) {
        return;
    }

    for(auto & fn : init_systems_.at(in_level_id)) {
        fn(in_parameters);
    }
}

void WSystemsRunner::RunPreSystems(const wid::WAssetId & in_level_id,
                                   const WSystemParameters & in_parameters) const {
    if (!pre_systems_.contains(in_level_id)) return;

    for (auto & fn : pre_systems_.at(in_level_id)) {
        fn(in_parameters);
    }
}

void WSystemsRunner::RunPostSystems(const wid::WAssetId & in_level_id,
                                    const WSystemParameters & in_parameters) const {
    if (!post_systems_.contains(in_level_id)) return;

    for (auto & fn : post_systems_.at(in_level_id)) {
        fn(in_parameters);
    }
}

void WSystemsRunner::RunEndSystems(const wid::WAssetId & in_level_id,
                                   const WSystemParameters & in_parameters) const {
    if (!end_systems_.contains(in_level_id)) return;

    for (auto & fn : end_systems_.at(in_level_id)) {
        fn(in_parameters);
    }
}

wid::WLevelSystemId WSystemsRunner::AddSystem(Systems & out_system,
                                         const ESystemLocation & in_location,
                                         wid::WAssetId const & in_level_id,
                                         const wid::WSystemId & in_system_id,
                                         const WSystemFn & in_system) {

    wid::WLevelSystemId lvlsysid{in_level_id, in_system_id};

    out_system[in_level_id].Insert(in_system_id.GetId(), in_system);

    systemid_location_[lvlsysid] = in_location;

    return lvlsysid;
        
}



