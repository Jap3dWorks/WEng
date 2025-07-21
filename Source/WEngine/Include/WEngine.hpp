#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TRef.hpp"

#include "IRender.hpp"
#include "ILevel.hpp"
#include "ILevelRegister.hpp"
#include "IImporterRegister.hpp"

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

    void run();

    static WEngine DefaultCreate();

    TRef<IImporterRegister> ImportersRegister() noexcept;

    TRef<IRender> Render() noexcept;

    TRef<ILevel> CurrentLevel() noexcept;

    TRef<WAssetManagerFacade> AssetManager() noexcept;

private:

    // void InitializeObjectManager();

    // void InitializeImporters();

    // void InitializeRender();

    std::unique_ptr<IRender> render_;

    std::unique_ptr<IImporterRegister> importers_register_;

    std::unique_ptr<ILevelRegister> level_register_;

    std::unique_ptr<WAssetManagerFacade> asset_manager_;

    bool close{false};

};

