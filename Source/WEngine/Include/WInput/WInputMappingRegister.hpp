#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TEvent.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WStructs/WEngineStructs.hpp"
#include "WAssets/WInputMappingAsset.hpp"
#include "WAssets/WActionAsset.hpp"
#include "WCore/TStack.hpp"

#include <unordered_map>

class WENGINE_API WInputMappingRegister {
private:

    using EventType = TEvent<const WInputValuesStruct &, const WActionStruct &>;

    using MappingAssetsSTackType = TStack<WAssetId>;

    using ActionEventsType = std::unordered_map<WAssetId, EventType>;

public:

    constexpr WInputMappingRegister() noexcept = default;

    constexpr virtual ~WInputMappingRegister() noexcept = default;

    constexpr WInputMappingRegister(const WInputMappingRegister &) noexcept=default;

    constexpr WInputMappingRegister(WInputMappingRegister &&) noexcept=default;

    WInputMappingRegister & operator=(const WInputMappingRegister &)=default;

    WInputMappingRegister & operator=(WInputMappingRegister &&)=default;

    void Emit(const WInputValuesStruct & input, const WAssetDb & asset_db) {
        for (auto & mid : mapping_assets_stack_) {
            
            const WInputMapStruct & inputmap =
                asset_db.Get<WInputMappingAsset>(mid)->InputMap();

            if (inputmap.map.contains(input.input)) {
                for(const auto &aid : inputmap.map.at(input.input)) {
                    action_events_[aid].Emit(
                        input,
                        asset_db.Get<WActionAsset>(aid)->ActionStruct()
                        );
                }

                // Resolve with the first detected input asset.
                return;
                
            }
        }
    }

    WEventId BindAction(const WAssetId & in_action, EventType::FnType && in_fn) {
        if (!action_events_.contains(in_action)) {
            action_events_[in_action]={};
        }

        return action_events_[in_action].Subscribe(std::move(in_fn));
    }

    void ClearAction(const WAssetId & in_action) {
        action_events_[in_action].Clear();
    }

    void UnbindAction(const WAssetId & in_action, const WEventId & in_event) {
        action_events_[in_action].Unsubscribe(in_event);
    }

    void PutInputMapping(const WAssetId & in_asset) {
        mapping_assets_stack_.Put(in_asset);
    }

    const WAssetId & TopInputMapping() const {
        return mapping_assets_stack_.Top();
    }

    void PopInputMapping() {
        mapping_assets_stack_.Pop();
    }

    bool ContainsInputMapping(const WAssetId & in_id) {
        return mapping_assets_stack_.Contains(in_id);
    }

    /**
     * @Brief returns InputMapping priority value, 0 higgest priority.
     */
    std::size_t PriorityInputMapping(const WAssetId & in_value) {
        return mapping_assets_stack_.Position(in_value);
    }

    void ClearInputMapping() {
        mapping_assets_stack_.Clear();
    }

private:

    MappingAssetsSTackType mapping_assets_stack_;
    ActionEventsType action_events_;
    
};
