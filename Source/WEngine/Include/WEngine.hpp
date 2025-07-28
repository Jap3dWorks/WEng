#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TRef.hpp"

#include "WEngineInterfaces/IRender.hpp"
#include "WLevel/WLevel.hpp"
#include "WImporterRegister.hpp"
#include "WLevelRegister/WLevelRegister.hpp"
#include "WObjectDb/WAssetDb.hpp"

#include <memory>

class WObjectDb;

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

    void StartupLevel(const WId & in_id) noexcept;

    void LoadLevel(const WId& in_level);

    WImporterRegister & ImportersRegister() noexcept;

    TRef<IRender> Render() noexcept;

    WAssetDb & AssetManager() noexcept;

private:

    void InitializeLevel(WLevel * in_level);

    struct LevelInfo {
        WId current_level{0};
        bool loaded{false};
        WLevel level{};
    } level_info_;

    struct StartupInfo {
        WId startup_level{0};
    } startup_info_;

    std::unique_ptr<IRender> render_;

    WImporterRegister importers_register_;

    WLevelRegister level_register_;

    WAssetDb asset_manager_;

    bool close{false};

};

