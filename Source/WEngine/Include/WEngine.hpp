#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TRef.hpp"
#include "WEngineInterfaces/IRender.hpp"
#include "WLevel/WLevel.hpp"
#include "WImporterRegister.hpp"
#include "WLevelRegister/WLevelRegister.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WStructs/WEngineStructs.hpp"
#include "WInput/WInputMappingRegister.hpp"

#include <memory>

class WENGINE_API WEngine
{

public:

    WEngine(std::unique_ptr<IRender> && in_render);

    virtual ~WEngine();

    WEngine(const WEngine & other) = delete;

    WEngine(WEngine && other) noexcept;

    WEngine & operator=(const WEngine & other) = delete;

    WEngine & operator=(WEngine && other) noexcept;

    static WEngine DefaultCreate();

    void run();

    void StartupLevel(const WLevelId & in_id) noexcept;

    void LoadLevel(const WLevelId & in_level);

    void UnloadLevel();

    WLevelRegister & LevelRegister() noexcept {
        return level_register_;
    }

    const WLevelRegister & LevelRegister() const noexcept {
        return level_register_;
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

    TRef<IRender> Render() noexcept;

    WAssetDb & AssetManager() noexcept;

    bool Initialize();

    void Destroy();

private:

    bool InitializeWindow();

    void DestroyWindow();

    void InitializeLevel(WLevel * in_level);

    static void FrameBufferSizeCallback(GLFWwindow * in_window, int width, int height);

    static void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods);

    struct LevelInfo {
        WLevelId current_level{0};
        bool loaded{false};
        WLevel level{};
    } level_info_;

    struct StartupInfo {
        WLevelId startup_level{0};
    } startup_info_;

    struct EngineStatus {
        bool close{false};
    }engine_status_;

    WWindowStruct window_;

    std::unique_ptr<IRender> render_;

    WImporterRegister importers_register_;

    WLevelRegister level_register_;

    WAssetDb asset_manager_;

    WInputMappingRegister input_mapping_register_;

};

