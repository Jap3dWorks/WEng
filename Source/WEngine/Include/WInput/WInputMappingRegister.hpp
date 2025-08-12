#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TEvent.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WStructs/WEngineStructs.hpp"
#include "WAssets/WInputMappingAsset.hpp"
#include "WAssets/WActionAsset.hpp"

#include <unordered_map>
#include <vector>

class WENGINE_API WInputMappingRegister {
private:

    using EventType = TEvent<const WInputValuesStruct &, const WActionStruct &>;

    using MappingAssetsQueueType = std::vector<WAssetId>;

    using ActionEventsType = std::unordered_map<WAssetId, EventType>;

public:

    void Emit(const WInputValuesStruct & input, const WAssetDb & asset_db) {
        // Iterate in reverse order (queue).
        for (auto ma_it = mapping_assets_queue.rbegin();
             ma_it != mapping_assets_queue.rend();
             ma_it++) {
            
            const WInputMapStruct & inputmap =
                asset_db.Get<WInputMappingAsset>(*ma_it)->InputMap();

            if (inputmap.map.contains(input.input)) {
                for(const auto &aid : inputmap.map.at(input.input)) {
                    action_events[aid].Emit(
                        input,
                        asset_db.Get<WActionAsset>(aid)->ActionStruct()
                        );
                }

                // Resolve with the first detected input asset.
                return;
                
            }
        }
    }

    void Put(const WInputMappingAsset & in_asset) {
        mapping_assets_queue.push_back(in_asset.WID());
    }

    const WAssetId & Top() const {
        return mapping_assets_queue.back();
    }

    void Pop() {
        mapping_assets_queue.resize(
            mapping_assets_queue.size() - 1
            );
    }

    // TODO
    bool Contains() {
        return false;
    }

    /** 0 higgest priority */
    std::uint32_t PriorityValue(const WAssetId & in_value) {
        return 999999;
    }
    

private:

    // TODO TQueue
    MappingAssetsQueueType mapping_assets_queue{};
    ActionEventsType action_events{};
    
};
