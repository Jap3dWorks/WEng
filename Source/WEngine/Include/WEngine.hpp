#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TRef.hpp"

#include "WEngineInterfaces/IRender.hpp"
#include "WEngineInterfaces/ILevel.hpp"
#include "WEngineInterfaces/ILevelRegister.hpp"
#include "WEngineInterfaces/IImporterRegister.hpp"

#include <memory>

class WObjectManager;
class WImporterRegister;
class WImporter;
class WAssetManagerFacade;

class WENGINE_API WEngine
{

public:

    WEngine(std::unique_ptr<IRender> && in_render,
            std::unique_ptr<IImporterRegister> && in_importers_register,
            std::unique_ptr<ILevelRegister> && in_level_register,
            std::unique_ptr<WAssetManagerFacade> && in_asset_manager);

    virtual ~WEngine();

    WEngine(const WEngine & other) = delete;

    WEngine(WEngine && other) = default;

    WEngine & operator=(const WEngine & other) = delete;

    WEngine & operator=(WEngine && other) = default;

    static WEngine DefaultCreate();

    void run();

    void StartupLevel(const WId & in_id) noexcept;

    void LoadLevel(const WId& in_level);

    TRef<IImporterRegister> ImportersRegister() noexcept;

    TRef<IRender> Render() noexcept;

    TRef<ILevel> CurrentLevel() noexcept;

    TRef<WAssetManagerFacade> AssetManager() noexcept;

private:

    struct LevelInfo {
        WId current_level{0};
        bool level_loaded{false};
    } level_info_;

    struct StartupInfo {
        WId startup_level{0};
    } startup_info_;

    std::unique_ptr<IRender> render_;

    std::unique_ptr<IImporterRegister> importers_register_;

    std::unique_ptr<ILevelRegister> level_register_;

    std::unique_ptr<WAssetManagerFacade> asset_manager_;

    bool close{false};

};

