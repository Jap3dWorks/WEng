#pragma once

#include "WAssets/Level.hpp"
#include "WSystem/SystemExecuter.hpp"


namespace weng::systems_lib {

    inline void RunLevelSystems(
        WEngine * engine,
        wsm::SystemExecuter & executer,
        wsm::ESystemLocation location,
        was::Level & level
        ) {

        executer.RunLevelSystems(
            location,
            wcr::wid::nullid,
            {engine, &level});
    
        executer.RunLevelSystems(
            location,
            level.Get_asset_id(),
            {engine, &level}
            );


    }

}
