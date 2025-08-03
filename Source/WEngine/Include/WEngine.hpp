#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TRef.hpp"

#include "WEngineInterfaces/IRender.hpp"
#include "WLevel/WLevel.hpp"
#include "WImporterRegister.hpp"
#include "WLevelRegister/WLevelRegister.hpp"
#include "WObjectDb/WAssetDb.hpp"

#include <memory>

class WENGINE_API WEngine
{

public:

    WEngine(std::unique_ptr<IRender> && in_render);

    virtual ~WEngine();

    WEngine(const WEngine & other) = delete;

    WEngine(WEngine && other) = default;

    WEngine & operator=(const WEngine & other) = delete;

    WEngine & operator=(WEngine && other) = default;

    static WEngine DefaultCreate();

    void run();

    WLevelRegister & LevelRegister() noexcept { return level_register_; }

    const WLevelRegister & LevelRegister() const noexcept { return level_register_; }

    void StartupLevel(const WLevelId & in_id) noexcept;

    void LoadLevel(const WLevelId & in_level);

    void UnloadLevel();

    WImporterRegister & ImportersRegister() noexcept;

    TRef<IRender> Render() noexcept;

    WAssetDb & AssetManager() noexcept;

private:

    void InitializeLevel(WLevel * in_level);

    struct LevelInfo {
        WLevelId current_level{0};
        bool loaded{false};
        WLevel level{};
    } level_info_;

    struct StartupInfo {
        WLevelId startup_level{0};
    } startup_info_;

    std::unique_ptr<IRender> render_;

    WImporterRegister importers_register_;

    WLevelRegister level_register_;

    WAssetDb asset_manager_;

    bool close{false};

};

