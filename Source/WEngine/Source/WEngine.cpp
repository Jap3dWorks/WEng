#include "WEngine.hpp"
// #include <iostream>

#include "WObjectManager/WObjectManager.hpp"
#include "WImporters.hpp"
#include "WImportersRegister.hpp"
#include "WRender.hpp"

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif

#include <GLFW/glfw3.h>

WEngine WEngine::Create()
{
    return {};
}

WEngine::WEngine()
{
    InitializeObjectManager();
    InitializeImporters();
    InitializeRender();
}

WEngine::~WEngine()
{
    render_ = nullptr;
    asset_manager_ = nullptr;
    importers_register_ = nullptr;
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

TRef<WImportersRegister> WEngine::ImportersRegister() noexcept
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

void WEngine::InitializeObjectManager()
{
    asset_manager_ = std::make_unique<WObjectManager>();
}

void WEngine::InitializeImporters()
{
    importers_register_ = std::make_unique<WImportersRegister>();

    importers_register_->Register<WImportObj>(*asset_manager_.get());
    importers_register_->Register<WImportTexture>(*asset_manager_.get());
    
}

void WEngine::InitializeRender()
{
    render_ = std::make_unique<WRender>();
}

