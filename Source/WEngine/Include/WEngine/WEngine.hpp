#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TRef.hpp"
#include "WEngineInterfaces/IRender.hpp"
#include "WLevel/WLevel.hpp"
#include "WImporterRegister.hpp"
#include "WLevel/WLevelDb.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WStructs/WEngineStructs.hpp"
#include "WInput/WInputMappingRegister.hpp"
#include "WSystems/WSystemsRegister.hpp"
#include "WSystems/WSystemsRunner.hpp"

#include <memory>

class WENGINE_API WEngine
{
private:

    struct LevelInfoStruct {
        WLevelId current_level{0};
        bool loaded{false};
        WLevel level{};
    };

    struct StartupInfo {
        WLevelId startup_level{0};
    };

public:

    WEngine(std::unique_ptr<IRender> && in_render);

    virtual ~WEngine();

    WEngine(const WEngine & other) = delete;

    WEngine(WEngine && other) noexcept;

    WEngine & operator=(const WEngine & other) = delete;

    WEngine & operator=(WEngine && other) noexcept;

    static WEngine DefaultCreate();

    void Run();

    void StartupLevel(const WLevelId & in_id) noexcept;

    WLevelId StartupLevel() const noexcept {
        return startup_info_.startup_level;
    }

    /** Set a level to be loaded when possible */
    void MarkLoadLevel(const WLevelId & in_level_id);

    const LevelInfoStruct & LevelInfo() const noexcept {
        return level_info_;
    }

    WLevelDb & LevelRegister() noexcept {
        return level_db_;
    }

    const WLevelDb & LevelRegister() const noexcept {
        return level_db_;
    }

    WImporterRegister & ImportersRegister() noexcept {
        return importers_register_;
    }

    const WImporterRegister & ImportersRegister() const noexcept {
        return importers_register_;
    }

    WInputMappingRegister & InputMappingRegister() noexcept {
        return input_mapping_register_;
    }

    const WInputMappingRegister & InputMappingRegister() const noexcept {
        return input_mapping_register_;
    }

    const WEngineCycleStruct & EngineCycle() const noexcept {
        return engine_cycle_;
    }

    WLevelSystemId AddInitSystem(const WLevelId & in_level_id, const char * in_system_name);

    WLevelSystemId AddPreSystem(const WLevelId & in_level_id, const char * in_system_name);

    WLevelSystemId AddPostSystem(const WLevelId & in_level_id, const char * in_system_name);

    WLevelSystemId AddEndSystem(const WLevelId & in_level_id, const char * in_system_name);

    TRef<IRender> Render() noexcept;

    WAssetDb & AssetManager() noexcept;

    bool Initialize();

    void Destroy();

private:

    bool InitializeWindow();

    void DestroyWindow();

    void InitializeLevel(WLevel * in_level);

    void UpdateEngineCycleStruct();

    static void FrameBufferSizeCallback(GLFWwindow * in_window, int width, int height);

    static void KeyCallback(GLFWwindow * in_window, int in_key, int in_scancode, int in_action, int in_mods);

    static void CursorCallback(GLFWwindow * in_window, double in_x, double in_y);

    void LoadLevel(WLevel & in_level);

    void UnloadLevel(WLevel & in_level);

    LevelInfoStruct level_info_{};

    StartupInfo startup_info_{};

    struct EngineStatus {
        bool close{false};
    }engine_status_;

    WWindowStruct window_{};

    WEngineCycleStruct engine_cycle_{};

    std::unique_ptr<IRender> render_{};

    WAssetDb asset_db_{};
    WLevelDb level_db_{};

    WSystemsRegister systems_reg_{};
    WSystemsRunner systems_runner_{};

    WInputMappingRegister input_mapping_register_{};

    WImporterRegister importers_register_{};

};

