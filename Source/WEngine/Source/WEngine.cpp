#include "WEngine.h"
// #include <iostream>

#include "WObjectManager/WObjectManager.h"
#include "WImporters.h"
#include "WImportersRegister.h"
#include "WRender.h"

WEngine::WEngine() {
    InitializeObjectManager();
    InitializeImporters();
    InitializeRender();
}

void WEngine::run() {}

TRef<WImportersRegister> WEngine::ImportersRegister() noexcept
{
    return {importers_register_.get()};
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
