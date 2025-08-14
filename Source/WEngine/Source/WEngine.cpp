#include "WEngine.hpp"

#include "WEngineInterfaces/IRender.hpp"
#include "WLevel/WLevel.hpp"

#include "WImporters.hpp"
#include "WImporterRegister.hpp"
#include "WStructs/WEngineStructs.hpp"
#include "WVulkan/WVkRender.hpp"
#include "WLevel/WLevelDb.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WEngineObjects/WEntity.hpp"
#include "WEngineObjects/WComponent.hpp"
#include "WComponents/WTransformComponent.hpp"
#include "WComponents/WCameraComponent.hpp"
#include "WInput/WInputLib.hpp"
#include "WLog.hpp"

#include "WRenderLevelLib.hpp"

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

WEngine WEngine::DefaultCreate()
{
    WEngine result(std::make_unique<WVkRender>());

    result.Initialize();

    result.ImportersRegister().Register<WImportObj>();
    result.ImportersRegister().Register<WImportTexture>();

    return result;
}

WEngine::WEngine(std::unique_ptr<IRender> && in_render) :
    level_info_(),
    startup_info_(),
    engine_status_(),
    window_(),
    render_(std::move(in_render)),
    importers_register_(),
    level_db_(),
    asset_db_(),
    input_mapping_register_()
{}

WEngine::~WEngine()=default;

WEngine::WEngine(WEngine && other) noexcept :
    level_info_(std::move(other.level_info_)),
    startup_info_(std::move(other.startup_info_)),
    engine_status_(std::move(other.engine_status_)),
    window_(std::move(other.window_)),
    render_(std::move(other.render_)),
    importers_register_(std::move(other.importers_register_)),
    level_db_(std::move(other.level_db_)),
    asset_db_(std::move(other.asset_db_)),
    input_mapping_register_(std::move(other.input_mapping_register_))
{
    if (window_.window) {
        glfwSetWindowUserPointer(window_.window, this);
    }

    other.window_ = {};
}

WEngine & WEngine::operator=(WEngine && other) noexcept {
    if (this != &other) {
        level_info_ = std::move(other.level_info_);
        startup_info_ = std::move(other.startup_info_);
        engine_status_ = std::move(other.engine_status_);
        window_ = std::move(other.window_);
        render_ = std::move(other.render_);
        importers_register_ = std::move(other.importers_register_);
        level_db_ = std::move(other.level_db_);
        asset_db_ = std::move(other.asset_db_);
        input_mapping_register_ = std::move(other.input_mapping_register_);

        if (window_.window) {
            glfwSetWindowUserPointer(window_.window, this);
        }

        other.window_ = {};
    }

    return *this;
}

bool WEngine::Initialize() {
    InitializeWindow();

    render_->Window(window_.window);
    render_->Initialize();

    return true;
}

void WEngine::Destroy() {
    render_->Destroy();
    DestroyWindow();
}

void WEngine::run()
{
    assert(startup_info_.startup_level);
    
    level_info_.current_level = startup_info_.startup_level;
    level_info_.loaded = false;

    // TODO Create WSystem to process the components (local WSystems, global WSystems (engine)).

    while(!glfwWindowShouldClose(window_.window)) {
        UpdateEngineCycleStruct();
        
        glfwPollEvents();
        
        if (!level_info_.loaded) {
            // TODO run End Systems

            Render()->WaitIdle();
            UnloadLevel();
            LoadLevel(level_info_.current_level);

            level_info_.level.Init(this);

            // TODO update static transforms WStaticTransformComponent

            system_db_.RunInitSystems(this);
            system_db_.RunInitLevelSystems(&level_info_.level, this);
        }
        else
        {
            system_db_.RunPreSystems(this);
            system_db_.RunPreLevelSystems(&level_info_.level, this);

            system_db_.RunPostSystems(this);
            system_db_.RunPostLevelSystems(&level_info_.level, this);

            // // Update Render Camera
            // level_info_.level.ForEachComponent<WCameraComponent> (
            //     [this] (WCameraComponent * cam) {
                    
            //         WTransformComponent * ts =
            //             level_info_.level.GetComponent<WTransformComponent>(
            //                 cam->EntityId()
            //                 );

            //         // ts->TransformStruct().position.x =
            //         //     ts->TransformStruct().position.x + .0001f;

            //         Render()->UpdateCamera(
            //             cam->CameraStruct(),
            //             ts->TransformStruct()
            //             );
            //     }
            //     );

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

    level_info_.level = level_db_.Get(in_level);

    // Initialize new level
    WRenderLevelLib::InitializeResources(
        render_.get(),
        &level_info_.level,
        // level_info_.level.EntityComponentDb(),
        asset_db_
        );

    level_info_.loaded = true;

}

void WEngine::UnloadLevel() {
    if (level_info_.loaded) {
        WRenderLevelLib::ReleaseRenderResources(
            render_.get(),
            &level_info_.level,
            asset_db_
            );
    }

    level_info_.loaded = false;
}

void WEngine::StartupLevel(const WLevelId& in_id) noexcept {
    startup_info_.startup_level = in_id;
}

TRef<IRender> WEngine::Render() noexcept
{
    return render_.get();
}

WAssetDb & WEngine::AssetManager() noexcept {
    return asset_db_;
}

bool WEngine::InitializeWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window_.window = glfwCreateWindow(
        800,
        600,
        "WEngine Title",
        nullptr,
        nullptr
        );

    glfwSetWindowUserPointer(window_.window, this);

    glfwSetFramebufferSizeCallback(
        window_.window,
        &FrameBufferSizeCallback
        );

    glfwSetKeyCallback(
        window_.window,
        &KeyCallback
        );

    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window_.window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    // glfwSetCursorPosCallback(window_.window, );

    return true;
}

void WEngine::DestroyWindow() {
    glfwDestroyWindow(window_.window);
    glfwTerminate();
}

void WEngine::UpdateEngineCycleStruct() {
    double seconds = glfwGetTime();

    engine_cycle_.DeltaTime = seconds = engine_cycle_.TotalTime;
    engine_cycle_.TotalTime = seconds;
    engine_cycle_.fps = 1 / engine_cycle_.DeltaTime;
}

void WEngine::FrameBufferSizeCallback(GLFWwindow* in_window, int in_width, int in_height)
{
    auto app = reinterpret_cast<WEngine*>(glfwGetWindowUserPointer(in_window));

    app->window_.width = in_width;
    app->window_.height = in_height;

    app->render_->Rescale(
        static_cast<std::uint32_t>(in_width),
        static_cast<std::uint32_t>(in_height)
        );
}

void WEngine::KeyCallback(GLFWwindow * in_window, int key, int scancode, int action, int mods)
{
    auto app = reinterpret_cast<WEngine*>(glfwGetWindowUserPointer(in_window));

    WInputMode imd = WInputLib::ToWInputMode(key, action);

    WFLOG("Key Callback {}, {}, {}, {}",
          key, scancode, action, mods);
    
    WInputValuesStruct ival = {imd, 1.f, {0,0}};

    // TODO: Store last cycle presed keys?
    
    // TODO: Also pass EngineData (delta time).
    // TODO: Pass CycleData
    app->input_mapping_register_.Emit(ival, app);
}
