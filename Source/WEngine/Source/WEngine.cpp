#include "WEngine.hpp"

#include "IImporterRegister.hpp"
#include "ILevelRegister.hpp"
#include "ILevel.hpp"
#include "IRender.hpp"

#include "WImporters.hpp"
#include "WImporterRegister.hpp"
#include "WRender.hpp"
#include "WLevelRegister/WLevelRegister.hpp"
#include "WObjectManager/WAssetManagerFacade.hpp"

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif

#include <GLFW/glfw3.h>

WEngine WEngine::DefaultCreate()
{
    
    std::unique_ptr<WAssetManagerFacade> asset_manager =
        std::make_unique<WAssetManagerFacade>();

    std::unique_ptr<IImporterRegister> importer_reg =
        std::make_unique<WImporterRegister>();

    importer_reg->Register(std::make_unique<WImportObj>(*asset_manager.get()));
    importer_reg->Register(std::make_unique<WImportTexture>(*asset_manager.get()));

    return WEngine(
        std::make_unique<WRender>(),
        std::move(importer_reg),
        std::make_unique<WLevelRegister>(),
        std::move(asset_manager)
        );
}

WEngine::WEngine(std::unique_ptr<IRender> && in_render,
                 std::unique_ptr<IImporterRegister> && in_importers_register,
                 std::unique_ptr<ILevelRegister> && in_level_register,
                 std::unique_ptr<WAssetManagerFacade> && in_asset_manager) :
    render_(std::move(in_render)),
    importers_register_(std::move(in_importers_register)),
    level_register_(std::move(in_level_register)),
    asset_manager_(std::move(in_asset_manager))
{}

WEngine::~WEngine()
{
    render_ = nullptr;
    importers_register_ = nullptr;
    level_register_ = nullptr;
    asset_manager_ = nullptr;
}

void WEngine::run()
{
    // TODO Window out of WRender
    while(!glfwWindowShouldClose(Render()->Window())) {
        glfwPollEvents();

        

        // draw

        Render()->Draw();
    }

    Render()->WaitIdle();
}

TRef<IImporterRegister> WEngine::ImportersRegister() noexcept
{
    return importers_register_.get();
}

TRef<IRender> WEngine::Render() noexcept
{
    return render_.get();
}

TRef<ILevel> WEngine::CurrentLevel() noexcept {
    return nullptr;
}

TRef<WAssetManagerFacade> WEngine::AssetManager() noexcept {
    return asset_manager_.get();
}

