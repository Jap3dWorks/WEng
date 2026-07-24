#pragma once

#include "WCore/WConcepts.hpp"
#include "WCore/WCore.hpp"
#include "WCore/TRef.hpp"
#include "WInterfaces/IRender.hpp"
#include "WImporterRegister/WImporterRegister.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WCoreTypes/WEngineStructs.hpp"
#include "WInput/WInputMappingRegister.hpp"

#include "WSystems/WSystemsRegister.hpp"
#include "WSystems/WSystemsRunner.hpp"

#include "WWindow/WWindow.hpp"

#include <memory>

class WENGINE_API WEngine
{
private:

    struct LevelInfoStruct {
        wcr::wid::WAssetId current_level{0};
        bool loaded{false};
        was::Level level{};
    };

    struct StartupInfo {
        wcr::wid::WAssetId startup_level{0};
    };

public:

    WEngine(std::unique_ptr<IRender> && in_render);

    virtual ~WEngine()=default;

    WEngine(const WEngine & other) = delete;

    WEngine & operator=(const WEngine & other) = delete;

    WEngine(WEngine && other);

    WEngine & operator=(WEngine && other);

    void Run();

    void StartupLevel(const wcr::wid::WAssetId & in_id) noexcept;

    wcr::wid::WAssetId StartupLevel() const noexcept {
        return state_.startup_info.startup_level;
    }

    /** Set a level to be loaded when possible */
    void MarkLoadLevel(const wcr::wid::WAssetId & in_level_id);

    const LevelInfoStruct & LevelInfo() const noexcept {
        return state_.level_info;
    }

    wim::imp_register::WImporterRegister & ImportersRegister() noexcept {
        return state_.importers_register;
    }

    const wim::imp_register::WImporterRegister & ImportersRegister() const noexcept {
        return state_.importers_register;
    }

    WInputMappingRegister & InputMappingRegister() noexcept {
        return state_.input_mapping_register;
    }

    const WInputMappingRegister & InputMappingRegister() const noexcept {
        return state_.input_mapping_register;
    }

    const WEngineCycleStruct & EngineCycle() const noexcept {
        return state_.engine_cycle;
    }

    wcr::wid::WLevelSystemId AddInitSystem(const wcr::wid::WAssetId & in_level_id, std::string_view in_system_name);
    wcr::wid::WLevelSystemId AddPreSystem(const wcr::wid::WAssetId & in_level_id, std::string_view in_system_name);
    wcr::wid::WLevelSystemId AddPostSystem(const wcr::wid::WAssetId & in_level_id, std::string_view in_system_name);
    wcr::wid::WLevelSystemId AddEndSystem(const wcr::wid::WAssetId & in_level_id, std::string_view in_system_name);

    template<CCallable<void, WSystemsRegister &> RFn>
    constexpr void RegSystems(RFn && in_fn) {
        in_fn(state_.systems_reg);
    }

    TRef<IRender> Render() noexcept;

    WAssetDb & AssetManager() noexcept;

private:

    void Initialize();

    void InitializeLevel(was::Level * in_level);

    void UpdateEngineCycleStruct();

    void LoadLevel(was::Level & in_level);

    void UnloadLevel(was::Level & in_level);

    static void FrameBufferSizeCallback(
        wdw::WWindow * in_window,
        int width,
        int height);

    static void KeyCallback(
        wdw::WWindow * in_window,
        int in_key,
        int in_scancode,
        int in_action,
        int in_mods);

    static void CursorCallback(
        wdw::WWindow * in_window,
        double in_x,
        double in_y);

private:

    /**
     * State Struct helps to reduce code in move semanthics.
     */
    struct State{

        LevelInfoStruct level_info{};

        StartupInfo startup_info{};

        struct EngineStatus {
            bool close{false};
        } engine_status{};

        wdw::WWindow window{};

        WEngineCycleStruct engine_cycle{};

        std::unique_ptr<IRender> render{nullptr};

        WAssetDb asset_db{};

        WSystemsRegister systems_reg{};
        WSystemsRunner systems_runner{};

        WInputMappingRegister input_mapping_register{};

        wim::imp_register::WImporterRegister importers_register{};

    } state_{};

};

