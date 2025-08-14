#include "WSystems/WSystemDb.hpp"
#include "WCore/WCore.hpp"
#include "WLevel/WLevel.hpp"

WSystemId WSystemDb::AddInitSystem(const WSystem::WSystemFn & in_system) {
    return AddSystem(init_systems_, ESystemLocation::INIT, in_system);
}

WSystemId WSystemDb::AddPreSystem(const WSystem::WSystemFn & in_system) {
    return AddSystem(pre_systems_, ESystemLocation::INIT, in_system);
}

WSystemId WSystemDb::AddPostSystem(const WSystem::WSystemFn & in_system) {
    return AddSystem(post_systems_, ESystemLocation::POST, in_system);
}

WSystemId WSystemDb::AddEndSystem(const WSystem::WSystemFn & in_system) {
    return AddSystem(end_systems_, ESystemLocation::END, in_system);
}

WSystemId WSystemDb::AddInitLevelSystem(const WLevelId & in_level_id,
                                        const WSystem::WLevelSystemFn & in_system) {
    return AddLevelSystem(init_level_systems_, ESystemLocation::LEVEL_PRE, in_level_id, in_system);
}

WSystemId WSystemDb::AddPreLevelSystem(const WLevelId & in_level_id,
                                       const WSystem::WLevelSystemFn & in_system) {
    return AddLevelSystem(pre_level_systems_, ESystemLocation::LEVEL_PRE, in_level_id, in_system);
}

WSystemId WSystemDb::AddPostLevelSystem(const WLevelId & in_level_id,
                                        const WSystem::WLevelSystemFn & in_system) {
    return AddLevelSystem(post_level_systems_, ESystemLocation::LEVEL_POST, in_level_id, in_system);
}

WSystemId WSystemDb::AddEndLevelSystem(const WLevelId & in_level_id,
                                       const WSystem::WLevelSystemFn & in_system) {
    return AddLevelSystem(end_level_systems_, ESystemLocation::LEVEL_END, in_level_id, in_system);
}

void WSystemDb::RemoveSystem(const WSystemId & in_id) {
    switch(systemid_location_[in_id]) {
    case ESystemLocation::INIT:
        init_systems_.Remove(in_id.GetId());
        break;
    case ESystemLocation::PRE:
        pre_systems_.Remove(in_id.GetId());
        break;
    case ESystemLocation::POST:
        post_systems_.Remove(in_id.GetId());
        break;
    case ESystemLocation::END:
        end_systems_.Remove(in_id.GetId());
        break;
    case ESystemLocation::LEVEL_INIT:
        init_level_systems_[systemid_levelid_[in_id]].Remove(in_id.GetId());
        systemid_levelid_.extract(in_id);
        break;
    case ESystemLocation::LEVEL_PRE:
        pre_level_systems_[systemid_levelid_[in_id]].Remove(in_id.GetId());
        systemid_levelid_.extract(in_id);
        break;
    case ESystemLocation::LEVEL_POST:
        post_level_systems_[systemid_levelid_[in_id]].Remove(in_id.GetId());
        systemid_levelid_.extract(in_id);
        break;
    case ESystemLocation::LEVEL_END:
        end_level_systems_[systemid_levelid_[in_id]].Remove(in_id.GetId());
        systemid_levelid_.extract(in_id);
        break;
    }

    systemid_location_.extract(in_id);
}

void WSystemDb::Clear() {
    pre_systems_.Clear();
    post_systems_.Clear();
    pre_level_systems_.clear();
    post_level_systems_.clear();

    id_pool_.Clear();

    systemid_location_.clear();
    systemid_levelid_.clear();

}

void WSystemDb::RunInitSystems(WEngine * in_engine) const {
    for(auto & fn : init_systems_) {
        fn(in_engine);
    }
}

void WSystemDb::RunPreSystems(WEngine * in_engine) const {

    for (auto & fn : pre_systems_) {
        fn(in_engine);
    }
}

void WSystemDb::RunPostSystems(WEngine * in_engine) const {
    for (auto & fn : post_systems_) {
        fn(in_engine);
    }
}

void WSystemDb::RunEndSystems(WEngine * in_engine) const {
    for (auto & fn : end_systems_) {
        fn(in_engine);
    }
}

void WSystemDb::RunInitLevelSystems(WLevel * in_level, WEngine * in_engine) const {
    for (auto & fn : init_level_systems_.at(in_level->WID())) {
        fn(in_level, in_engine);
    }
}

void WSystemDb::RunPreLevelSystems(WLevel * in_level, WEngine * in_engine) const {
    for(auto & fn : pre_level_systems_.at(in_level->WID())) {
        fn(in_level, in_engine);
    }
}

void WSystemDb::RunPostLevelSystems(WLevel * in_level, WEngine * in_engine) const {
    for (auto & fn : post_level_systems_.at(in_level->WID())) {
        fn(in_level, in_engine);
    }
}

void WSystemDb::RunEndLevelSystems(WLevel * in_level, WEngine * in_engine) const {
    for (auto & fn : end_level_systems_.at(in_level->WID())) {
        fn(in_level, in_engine);
    }
}

WSystemId WSystemDb::AddSystem(Systems & out_system,
                               const ESystemLocation & in_location,
                               const WSystem::WSystemFn & in_system) {
    WSystemId sid = id_pool_.Generate();

    out_system.Insert(sid.GetId(), in_system);
    systemid_location_[sid] = in_location;

    return sid;   
}

WSystemId WSystemDb::AddLevelSystem(LevelSystems & out_system,
                                    const ESystemLocation & in_location,
                                    const WLevelId & in_level_id,
                                    const WSystem::WLevelSystemFn & in_system) {
    WSystemId sid = id_pool_.Generate();

    out_system[in_level_id].Insert(in_level_id.GetId(), in_system);
    systemid_location_[sid] = in_location;
    systemid_levelid_[sid] = in_level_id;

    return sid;
        
}


