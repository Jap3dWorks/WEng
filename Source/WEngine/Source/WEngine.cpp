#include "WEngine.hpp"

#include "WEngineInterfaces/ILevelRegister.hpp"
#include "WEngineInterfaces/ILevel.hpp"
#include "WEngineInterfaces/IRender.hpp"

#include "WImporters.hpp"
#include "WImporterRegister.hpp"
#include "WRender.hpp"
#include "WLevelRegister/WLevelRegister.hpp"
#include "WObjectManager/WAssetManager.hpp"
#include "WEngineObjects/WActor.hpp"
#include "WEngineObjects/WComponent.hpp"

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif

#include <GLFW/glfw3.h>

WEngine WEngine::DefaultCreate()
{
    WEngine result(std::make_unique<WRender>());

    result.ImportersRegister()->Register<WImportObj>();
    result.ImportersRegister()->Register<WImportTexture>();

    return result;
}

WEngine::WEngine(std::unique_ptr<IRender> && in_render) :
    render_(std::move(in_render)),
    importers_register_(),
    level_register_(),
    asset_manager_()
{}

WEngine::~WEngine()
{
    render_ = nullptr;
}

void WEngine::run()
{
    assert(startup_info_.startup_level);
    
    std::unique_ptr<ILevel> level = nullptr;

    level_info_.current_level = startup_info_.startup_level;
    level_info_.level_loaded = false;

    // TODO Window out of WRender
    while(!glfwWindowShouldClose(Render()->Window())) {
        glfwPollEvents();
        
        WEngineCycleData engine_cycle_data;

        if (!level_info_.level_loaded) {

            level = level_register_.Get(
                level_info_.current_level
                )->Clone();
            
            level->Init(engine_cycle_data);
            level_info_.level_loaded = true;
        }
        else {
            // Update Components
            level->ForEachComponent(
                WComponent::StaticClass(),
                [&level,
                 &engine_cycle_data](WComponent * in_component) {
                    in_component->OnUpdate(
                        engine_cycle_data
                        );
                });

            // Update Actors
            level->ForEachActor(
                WActor::StaticClass(),
                [&level,
                 &engine_cycle_data](WActor * in_actor) {
                    in_actor->OnUpdate(
                        engine_cycle_data
                        );
                });

            // draw
            Render()->Draw();
        }
    }

    Render()->WaitIdle();
}

void WEngine::LoadLevel(const WId & in_level) {
    
}

void WEngine::StartupLevel(const WId& in_id) noexcept {
    startup_info_.startup_level = in_id;
}

TRef<WImporterRegister> WEngine::ImportersRegister() noexcept
{
    return importers_register_;
}

TRef<IRender> WEngine::Render() noexcept
{
    return render_.get();
}

TRef<ILevel> WEngine::CurrentLevel() noexcept {
    return nullptr;
}

TRef<WAssetManager> WEngine::AssetManager() noexcept {
    return asset_manager_;
}

