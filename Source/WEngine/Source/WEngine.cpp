#include "WEngine.hpp"

#include "WEngineInterfaces/IRender.hpp"
#include "WLevel/WLevel.hpp"

#include "WImporters.hpp"
#include "WImporterRegister.hpp"
#include "WVulkan/WVkRender.hpp"
#include "WLevelRegister/WLevelRegister.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WEngineObjects/WEntity.hpp"
#include "WEngineObjects/WComponent.hpp"
#include "WComponents/WTransformComponent.hpp"
#include "WComponents/WCameraComponent.hpp"
#include "WLog.hpp"

#include "WRenderLevelLib.hpp"

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif

#include <GLFW/glfw3.h>

WEngine WEngine::DefaultCreate()
{
    std::unique_ptr<IRender> rndrptr = std::make_unique<WVkRender>();

    WEngine result(std::make_unique<WVkRender>());

    result.Render()->Initialize();

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

WEngine::WEngine()=default;

WEngine::~WEngine()=default;
// {
//     WFLOG("Desroy WEngine");
    
//     render_->Destroy();
//     render_ = nullptr;
// }

WEngine::WEngine(WEngine && other) noexcept=default;

WEngine & WEngine::operator=(WEngine && other) noexcept=default;

void WEngine::run()
{
    assert(startup_info_.startup_level);
    
    level_info_.current_level = startup_info_.startup_level;
    level_info_.loaded = false;

    // TODO Window out of WRender
    while(!glfwWindowShouldClose(Render()->Window())) {
        glfwPollEvents();
        
        WEngineCycleData engine_cycle_data;

        if (!level_info_.loaded) {
            Render()->WaitIdle();
            UnloadLevel();
            LoadLevel(level_info_.current_level);
        }
        else
        {
            // Update Render Camera
            level_info_.level.ForEachComponent<WCameraComponent> (
                [&engine_cycle_data, this] (WCameraComponent * cam) {
                    
                    WTransformComponent * ts =
                        level_info_.level.GetComponent<WTransformComponent>(
                            cam->EntityId()
                            );

                    Render()->UpdateCamera(
                        cam->CameraStruct(),
                        ts->TransformStruct()
                        );
                }
                );

            // Update transform Components
            level_info_.level.ForEachComponent<WTransformComponent>(
                [&engine_cycle_data, this](WTransformComponent * in_component) {
                    WEntityComponentId ecid = level_info_.level
                        .EntityComponentDb()
                        .EntityComponentId(
                            in_component->EntityId(),
                            in_component->Class()
                            );
                    
                    // TODO, Render()->UpdateUbo(ecid, in_component);

                });

            // draw
            Render()->Draw();
        }
    }
    
    Render()->WaitIdle();
    UnloadLevel();
}

void WEngine::LoadLevel(const WLevelId & in_level) {
    level_info_.loaded = false;
    level_info_.current_level = in_level;

    level_info_.level = level_register_.Get(in_level);

    // Initialize new level
    WRenderLevelLib::InitializeResources(
        render_.get(),
        level_info_.level.EntityComponentDb(),
        asset_manager_
        );

    level_info_.loaded = true;

    // TODO update static transforms WStaticTransformComponent
}

void WEngine::UnloadLevel() {
    if (level_info_.loaded) {
        WRenderLevelLib::ReleaseRenderResources(
            render_.get(),
            level_info_.level.EntityComponentDb(),
            asset_manager_
            );
    }

    level_info_.loaded = false;
}

void WEngine::StartupLevel(const WLevelId& in_id) noexcept {
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

