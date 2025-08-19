#include "WEngine/WEngine.hpp"

#include "WCore/WCore.hpp"
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

#include "WUtils/WRenderLevelUtils.hpp"

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

    // Register Wengine systems
    
    WSystems::WENGINE_WSYSTEMS_REG(result.systems_reg_);

    result.AddInitSystem(0, "System_InitRenderLevelResources");

    result.AddPreSystem(0, "System_PostUpdateRenderTransform");

    result.AddPostSystem(0, "System_PostUpdateRenderCamera");

    result.AddEndSystem(0, "System_EndRenderLevelResources");

    // TODO Plugins Modules Loading

    return result;
}

WEngine::WEngine(std::unique_ptr<IRender> && in_render) :
    level_info_(),
    startup_info_(),
    engine_status_(),
    window_(),
    engine_cycle_(),
    render_(std::move(in_render)),
    asset_db_(),
    level_db_(),
    systems_reg_(),
    systems_runner_(),
    input_mapping_register_(),
    importers_register_()    
{}

WEngine::~WEngine()=default;

WEngine::WEngine(WEngine && other) noexcept :
    level_info_(std::move(other.level_info_)),
    startup_info_(std::move(other.startup_info_)),
    engine_status_(std::move(other.engine_status_)),
    window_(std::move(other.window_)),
    engine_cycle_(std::move(other.engine_cycle_)),
    render_(std::move(other.render_)),
    asset_db_(std::move(other.asset_db_)),
    level_db_(std::move(other.level_db_)),
    systems_reg_(std::move(other.systems_reg_)),
    systems_runner_(std::move(other.systems_runner_)),
    input_mapping_register_(std::move(other.input_mapping_register_)),
    importers_register_(std::move(other.importers_register_))
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
        asset_db_ = std::move(other.asset_db_);
        level_db_ = std::move(other.level_db_);
        systems_reg_ = std::move(other.systems_reg_);
        systems_runner_ = std::move(other.systems_runner_);
        input_mapping_register_ = std::move(other.input_mapping_register_);
        importers_register_ = std::move(other.importers_register_);

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
    level_info_.level = level_db_.Get(level_info_.current_level);
    LoadLevel(level_info_.level);

    level_info_.loaded = true;

    while(!glfwWindowShouldClose(window_.window)) {
        UpdateEngineCycleStruct();
        glfwPollEvents();
        
        if (!level_info_.loaded) {
            UnloadLevel(level_info_.level);

            Render()->WaitIdle();

            level_info_.level = level_db_.Get(level_info_.current_level);
            LoadLevel(level_info_.level);

            level_info_.loaded = true;
        }
        else
        {
            systems_runner_.RunPreSystems(0, {this, &level_info_.level});
            systems_runner_.RunPreSystems(level_info_.level.WID(),
                                          {this, &level_info_.level});

            systems_runner_.RunPostSystems(0, {this, &level_info_.level});
            systems_runner_.RunPostSystems(level_info_.level.WID(),
                                           {this, &level_info_.level});

            Render()->Draw();
        }
    }
    
    Render()->WaitIdle();
    UnloadLevel(level_info_.level);
}

void WEngine::MarkLoadLevel(const WLevelId & in_level) {
    level_info_.current_level = in_level;
    level_info_.loaded = false;
}

void WEngine::LoadLevel(WLevel & in_level) {
    // TODO: register level systems

    systems_runner_.RunInitSystems(
        0, {this, &level_info_.level}
        );
    
    systems_runner_.RunInitSystems(
        level_info_.level.WID(),
        {this, &level_info_.level}
        );
}

void WEngine::UnloadLevel(WLevel & in_level) {

    systems_runner_.RunEndSystems(0, {this, &in_level});
    systems_runner_.RunEndSystems(in_level.WID(), {this, &in_level});

    // TODO deregister level systems

}

void WEngine::StartupLevel(const WLevelId& in_id) noexcept {
    startup_info_.startup_level = in_id;
}

WLevelSystemId WEngine::AddInitSystem(const WLevelId & in_level_id, const char * in_system_name) {
    WSystemId wsid = systems_reg_.GetId(in_system_name);
    return systems_runner_.AddInitSystem(
        in_level_id, wsid, systems_reg_.Get(wsid)
        );
}

WLevelSystemId WEngine::AddPreSystem(const WLevelId & in_level_id, const char * in_system_name) {
    WSystemId wsid = systems_reg_.GetId(in_system_name);
    return systems_runner_.AddPreSystem(
        in_level_id, wsid, systems_reg_.Get(wsid)
        );
}

WLevelSystemId WEngine::AddPostSystem(const WLevelId & in_level_id, const char * in_system_name) {
    WSystemId wsid = systems_reg_.GetId(in_system_name);
    return systems_runner_.AddPostSystem(
        in_level_id, wsid, systems_reg_.Get(wsid)
        );
}

WLevelSystemId WEngine::AddEndSystem(const WLevelId & in_level_id, const char * in_system_name) {
    WSystemId wsid = systems_reg_.GetId(in_system_name);
    return systems_runner_.AddEndSystem(
        in_level_id, wsid, systems_reg_.Get(wsid)
        );
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
    // TODO: Pass CycleData.
    app->input_mapping_register_.Emit(ival, app);
}
