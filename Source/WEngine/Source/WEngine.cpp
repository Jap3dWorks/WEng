#include "WEngine.hpp"

#include "WEngineInterfaces/IImporterRegister.hpp"
#include "WEngineInterfaces/ILevelRegister.hpp"
#include "WEngineInterfaces/ILevel.hpp"
#include "WEngineInterfaces/IRender.hpp"

#include "WImporters.hpp"
#include "WImporterRegister.hpp"
#include "WRender.hpp"
#include "WLevelRegister/WLevelRegister.hpp"
#include "WEngineObjects/WActor.hpp"
#include "WEngineObjects/WComponent.hpp"

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif

#include <GLFW/glfw3.h>

WEngine WEngine::DefaultCreate()
{
    
    std::unique_ptr<WAssetManagerFacade> asset_manager =
        std::make_unique<WAssetManagerFacade>();

    std::unique_ptr<IImporterRegister> importer_reg =
        std::make_unique<WImporterRegister>(*asset_manager.get());
    
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
    TOptionalRef<ILevel> level = level_register_->Get(level_register_->Current());
        
    // TODO Window out of WRender
    while(!glfwWindowShouldClose(Render()->Window())) {
        glfwPollEvents();

        // Update Components
        level->ForEachComponent(
            WComponent::StaticClass(),
            [&level](WComponent * in_component) {
                in_component->OnUpdate();
            });

        // Update Actors
        level->ForEachActor(
            WActor::StaticClass(),
            [&level](WActor * in_actor) {
                in_actor->OnUpdate();
            });

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

