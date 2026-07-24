#include "WEngine/WEngine.hpp"

#include "WCore/WCore.hpp"
#include "WEngineInterfaces/IRender.hpp"

// #include "WImporter/WImporterTexture.hpp"
// #include "WImporter/WImporterObj.hpp"
// #include "WImporterRegister/WImporterRegister.hpp"

#include "WCoreTypes/WEngineStructs.hpp"
#include "WVulkan/WVkRender.hpp"
#include "WObjectDb/WAssetDb.hpp"
// #include "WObjects/WEntity.hpp"
// #include "WObjects/WComponent.hpp"
// #include "WComponents/Transform.hpp"
// #include "WComponents/Camera.hpp"
#include "WInput/WInputLib.hpp"
#include "WLog.hpp"

#include "WEngRender/WEngRender.hpp"

// WEngine WEngine::DefaultCreate()
// {
//     WEngine result(std::make_unique<WVkRender>());

//     result.ImportersRegister().Register<wim::importer::WImporterObj>();
//     result.ImportersRegister().Register<wim::importer::WImportTexture>();

//     // Register Wengine systems
    
//     WSystems::WENGINE_WSYSTEMS_REG(result.state_.systems_reg);

//     // This must be the first included system
//     result.AddInitSystem(0, "SystemInit_InitializeTransformsMatrix");

//     result.AddInitSystem(0, "SystemInit_RenderLevelResources");

//     result.AddPostSystem(0, "SystemPost_UpdateRenderCamera");

//     result.AddEndSystem(0, "SystemEnd_RenderLevelResources");

//     // Default Assets
    

//     // TODO Plugins Modules Loading

//     return result;
// }

WEngine::WEngine(std::unique_ptr<IRender> && in_render)
{
    state_.render = std::move(in_render);
    Initialize();
}

WEngine::WEngine(WEngine && other):
    state_(std::move(other.state_))

{
    if (state_.window.IsValid()) {
        state_.window.SetWindowUserPtr(this);
    }

}

WEngine & WEngine::operator=(WEngine && other)
{
    if (this != &other) {
        state_ = std::move(other.state_);

        if (state_.window.IsValid()) {
            state_.window.SetWindowUserPtr(this);
        }
    }

    return *this;
}

void WEngine::Initialize() {
    state_.window.Initialize();
    state_.window.SetWindowUserPtr(this);

    state_.window.SetFramebufferSizeCallback(FrameBufferSizeCallback);
    state_.window.SetKeyCallback(KeyCallback);
    state_.window.SetCursorPosCallback(CursorCallback);

    state_.render->SetWindow(&state_.window);
}

void WEngine::Run()
{
    assert(state_.startup_info.startup_level.IsValid());

    state_.level_info.current_level = state_.startup_info.startup_level;
    state_.level_info.level =
        (state_.asset_db.Get<was::Level>(state_.level_info.current_level));

    LoadLevel(state_.level_info.level);

    state_.level_info.loaded = true;

    while(!state_.window.ShouldClose()) {
        UpdateEngineCycleStruct();
        state_.window.PollEvents();
        
        if (!state_.level_info.loaded) {
            UnloadLevel(state_.level_info.level);

            Render()->WaitIdle();

            state_.level_info.level =
                state_.asset_db.Get<was::Level>(
                state_.level_info.current_level);

            LoadLevel(state_.level_info.level);

            WLOG("Level Load Done.");

            state_.level_info.loaded = true;
        }
        else
        {
            state_.systems_runner
                .RunPreSystems(0, {this, &state_.level_info.level});

            state_.systems_runner
                .RunPreSystems(state_.level_info.level.Get_asset_id(),
                               {this, &state_.level_info.level});

            state_.systems_runner
                .RunPostSystems(0, {this, &state_.level_info.level});

            state_.systems_runner
                .RunPostSystems(state_.level_info.level.Get_asset_id(),
                                {this, &state_.level_info.level});

            Render()->Draw();
        }
    }
    
    Render()->WaitIdle();
    UnloadLevel(state_.level_info.level);
}

void WEngine::MarkLoadLevel(const wcr::wid::WAssetId & in_level) {
    state_.level_info.current_level = in_level;
    state_.level_info.loaded = false;
}

void WEngine::LoadLevel(was::Level & in_level) {
    // TODO register level systems

    WFLOG("[DEBUG] Run Engine Init Systems.");
    state_.systems_runner.RunInitSystems(
        0, {this, &state_.level_info.level}
        );

    WFLOG("[DEBUG] Run Level Init Systems.")
    state_.systems_runner.RunInitSystems(
        state_.level_info.level.Get_asset_id(),
        {this, &state_.level_info.level}
        );
}

void WEngine::UnloadLevel(was::Level & in_level) {

    state_.systems_runner.RunEndSystems(0, {this, &in_level});
    state_.systems_runner.RunEndSystems(in_level.Get_asset_id(), {this, &in_level});

    // TODO deregister level systems
    
}

void WEngine::StartupLevel(const wcr::wid::WAssetId& in_id) noexcept {
    state_.startup_info.startup_level = in_id;
}

// WLevelSystemId WEngine::AddInitSystem(const wcr::wid::WAssetId & in_level_id, const char * in_system_name) {
//     WSystemId wsid = state_.systems_reg.GetId(in_system_name);
//     return state_.systems_runner.AddInitSystem(
//         in_level_id, wsid, state_.systems_reg.Get(wsid)
//         );
// }

// WLevelSystemId WEngine::AddPreSystem(const wcr::wid::WAssetId & in_level_id, const char * in_system_name) {
//     WSystemId wsid = state_.systems_reg.GetId(in_system_name);
//     return state_.systems_runner.AddPreSystem(
//         in_level_id, wsid, state_.systems_reg.Get(wsid)
//         );
// }

// WLevelSystemId WEngine::AddPostSystem(const wcr::wid::WAssetId & in_level_id, const char * in_system_name) {
//     WSystemId wsid = state_.systems_reg.GetId(in_system_name);
//     return state_.systems_runner.AddPostSystem(
//         in_level_id, wsid, state_.systems_reg.Get(wsid)
//         );
// }

// WLevelSystemId WEngine::AddEndSystem(const wcr::wid::WAssetId & in_level_id, const char * in_system_name) {
//     WSystemId wsid = state_.systems_reg.GetId(in_system_name);
//     return state_.systems_runner.AddEndSystem(
//         in_level_id, wsid, state_.systems_reg.Get(wsid)
//         );
// }


wcr::wid::WLevelSystemId WEngine::AddInitSystem(const wcr::wid::WAssetId & in_level_id, std::string_view in_system_name) {
    wcr::wid::WSystemId wsid = state_.systems_reg.GetId(in_system_name);
    return state_.systems_runner.AddInitSystem(
        in_level_id, wsid, state_.systems_reg.Get(wsid)
        );
}

wcr::wid::WLevelSystemId WEngine::AddPreSystem(const wcr::wid::WAssetId & in_level_id, std::string_view in_system_name) {
    wcr::wid::WSystemId wsid = state_.systems_reg.GetId(in_system_name);
    return state_.systems_runner.AddPreSystem(
        in_level_id, wsid, state_.systems_reg.Get(wsid)
        );
}

wcr::wid::WLevelSystemId WEngine::AddPostSystem(const wcr::wid::WAssetId & in_level_id, std::string_view in_system_name) {
    wcr::wid::WSystemId wsid = state_.systems_reg.GetId(in_system_name);
    return state_.systems_runner.AddPostSystem(
        in_level_id, wsid, state_.systems_reg.Get(wsid)
        );
}

wcr::wid::WLevelSystemId WEngine::AddEndSystem(const wcr::wid::WAssetId & in_level_id, std::string_view in_system_name) {
    wcr::wid::WSystemId wsid = state_.systems_reg.GetId(in_system_name);
    return state_.systems_runner.AddEndSystem(
        in_level_id, wsid, state_.systems_reg.Get(wsid)
        );
}

TRef<IRender> WEngine::Render() noexcept
{
    return state_.render.get();
}

WAssetDb & WEngine::AssetManager() noexcept {
    return state_.asset_db;
}

void WEngine::UpdateEngineCycleStruct() {
    double seconds = glfwGetTime();

    state_.engine_cycle.DeltaTime = seconds - state_.engine_cycle.TotalTime;
    state_.engine_cycle.TotalTime = seconds;
    state_.engine_cycle.fps = 1 / state_.engine_cycle.DeltaTime;
}

void WEngine::FrameBufferSizeCallback(wdw::WWindow* in_window, int in_width, int in_height)
{
    auto app = reinterpret_cast<WEngine*>(in_window->GetWindowUserPtr());

    app->state_.render->Rescale(
        static_cast<std::uint32_t>(in_width),
        static_cast<std::uint32_t>(in_height)
        );
}

void WEngine::KeyCallback(wdw::WWindow * in_window, int key, int scancode, int action, int mods)
{
    auto app = reinterpret_cast<WEngine*>(in_window->GetWindowUserPtr());

    WInput imd = WInputLib::ToWInputMode(key, action);

    WInputValuesStruct ival = {imd, 1.f, {0,0}};

    app->state_.input_mapping_register.Emit(ival, app);
}

void WEngine::CursorCallback(wdw::WWindow * in_window, double in_x, double in_y) {
    auto app = reinterpret_cast<WEngine *>(in_window->GetWindowUserPtr());

    WInputValuesStruct ival {
        {EInputKey::Mouse_Move, EInputMode::None},
        0.f,
        {in_x, in_y}
    };

    app->state_.input_mapping_register.Emit(ival, app);
}
