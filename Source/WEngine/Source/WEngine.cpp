#include "WEngine.h"
// #include <iostream>

#include "WObjectManager/WObjectManager.h"
#include "WImporters.h"
#include "WImportersRegister.h"
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
    object_manager_ = nullptr;
    importers_register_ = nullptr;
}

void WEngine::run()
{
    // TODO Window out of WRender
    while(!glfwWindowShouldClose(Render()->WindowInfo().window)) {
        glfwPollEvents();

        

        // draw

        Render()->Draw();
    }

    Render()->DeviceWaitIdle();
}

TRef<WImportersRegister> WEngine::ImportersRegister() noexcept
{
    return importers_register_.get();
}

TRef<WRender> WEngine::Render() noexcept
{
    return render_.get();
}

void WEngine::InitializeObjectManager()
{
    object_manager_ = std::make_unique<WObjectManager>();
}

void WEngine::InitializeImporters()
{
    importers_register_ = std::make_unique<WImportersRegister>();

    importers_register_->Register<WImportObj>(*object_manager_.get());
    importers_register_->Register<WImportTexture>(*object_manager_.get());
    
}

void WEngine::InitializeRender()
{
    render_ = std::make_unique<WRender>();
}

