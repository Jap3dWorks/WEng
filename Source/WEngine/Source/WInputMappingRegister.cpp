#pragma once
#include "WEngine.hpp"

void WInputMappingRegister::Emit(const WInputValuesStruct & in_input, WEngine * in_engine) 
{
    for (auto & mid : mapping_assets_stack_) {
            
        const WInputMapStruct & inputmap =
            in_engine->AssetManager().Get<WInputMappingAsset>(mid)->InputMap();

        if (inputmap.map.contains(in_input.input)) {
            for(const auto &aid : inputmap.map.at(in_input.input)) {
                action_events_[aid].Emit(
                    in_input,
                    in_engine->AssetManager().Get<WActionAsset>(aid)->ActionStruct(),
                    in_engine
                    );
            }

            // Resolved with the first detected input asset.
            return;
                
        }
    }
}
