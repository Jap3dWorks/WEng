#include "WEngine.hpp"

#include "WEngineInterfaces/IRender.hpp"
#include "WLevel/WLevel.hpp"

#include "WImporters.hpp"
#include "WImporterRegister.hpp"
#include "WRender.hpp"
#include "WLevelRegister/WLevelRegister.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WEngineObjects/WActor.hpp"
#include "WEngineObjects/WComponent.hpp"
#include "WLog.hpp"

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif

#include <GLFW/glfw3.h>

WEngine WEngine::DefaultCreate()
{
    WEngine result(std::make_unique<WRender>());

    result.ImportersRegister().Register<WImportObj>();
    result.ImportersRegister().Register<WImportTexture>();

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
    
    level_info_.current_level = startup_info_.startup_level;
    level_info_.level_loaded = false;
    level_info_.level = level_register_.GetCopy(level_info_.current_level);

    // TODO Window out of WRender
    while(!glfwWindowShouldClose(Render()->Window())) {
        glfwPollEvents();
        
        WEngineCycleData engine_cycle_data;

        if (!level_info_.level_loaded) {

            level_info_.level = level_register_.GetCopy(
                level_info_.current_level
                );
            
            level_info_.level.Init(engine_cycle_data);
            level_info_.level_loaded = true;
        }
        else {
            // Update Components
            level_info_.level.ForEachComponent(
                WComponent::StaticClass(),
                [&engine_cycle_data](WComponent * in_component) {
                    in_component->OnUpdate(
                        engine_cycle_data
                        );
                });

            // Update Actors
            level_info_.level.ForEachActor(
                WActor::StaticClass(),
                [&engine_cycle_data](WActor * in_actor) {
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
    // TODO
    WFLOG("NOT IMPLEMENTED!");
}

void WEngine::StartupLevel(const WId& in_id) noexcept {
    startup_info_.startup_level = in_id;
}

WImporterRegister & WEngine::ImportersRegister() noexcept
{
    return importers_register_;
}

TRef<IRender> WEngine::Render() noexcept
{
    return render_.get();
}

WAssetDb & WEngine::AssetManager() noexcept {
    return asset_manager_;
}

