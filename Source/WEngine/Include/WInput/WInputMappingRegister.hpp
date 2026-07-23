#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TEvent.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WCoreTypes/WEngineStructs.hpp"
#include "WAssets/InputMapping.hpp"
#include "WAssets/Action.hpp"
#include "WCore/TStack.hpp"

#include <unordered_map>

class WEngine;

class WENGINE_API WInputMappingRegister {
private:

using EventType = TEvent<void(const WInputValuesStruct &, was::Action const *, WEngine *)>;

    using MappingAssetsSTackType = TStack<wcr::wid::WAssetId>;

    using ActionEventsType = std::unordered_map<wcr::wid::WAssetId, EventType>;

public:

    constexpr WInputMappingRegister() noexcept = default;

    constexpr virtual ~WInputMappingRegister() noexcept = default;

    constexpr WInputMappingRegister(const WInputMappingRegister &) noexcept=default;

    constexpr WInputMappingRegister(WInputMappingRegister &&) noexcept=default;

    WInputMappingRegister & operator=(const WInputMappingRegister &)=default;

    WInputMappingRegister & operator=(WInputMappingRegister &&)=default;

    void Emit(const WInputValuesStruct & in_input, WEngine * in_engine);

    wcr::wid::WEventId BindAction(const wcr::wid::WAssetId & in_action, EventType::FnType && in_fn) {
        if (!action_events_.contains(in_action)) {
            action_events_[in_action]={};
        }

        return action_events_[in_action].Subscribe(std::move(in_fn));
    }

    void ClearAction(const wcr::wid::WAssetId & in_action) {
        action_events_[in_action].Clear();
    }

    void UnbindAction(const wcr::wid::WAssetId & in_action, const wcr::wid::WEventId & in_event) {
        action_events_[in_action].Unsubscribe(in_event);
    }

    void PutInputMapping(const wcr::wid::WAssetId & in_asset) {
        mapping_assets_stack_.Put(in_asset);
    }

    const wcr::wid::WAssetId & TopInputMapping() const {
        return mapping_assets_stack_.Top();
    }

    void PopInputMapping() {
        mapping_assets_stack_.Pop();
    }

    bool ContainsInputMapping(const wcr::wid::WAssetId & in_id) {
        return mapping_assets_stack_.Contains(in_id);
    }

    /**
     * @Brief returns InputMapping priority value, 0 higgest priority.
     */
    std::size_t PriorityInputMapping(const wcr::wid::WAssetId & in_value) {
        return mapping_assets_stack_.Position(in_value);
    }

    void ClearInputMapping() {
        mapping_assets_stack_.Clear();
    }

private:

    MappingAssetsSTackType mapping_assets_stack_;
    ActionEventsType action_events_;
    
};
