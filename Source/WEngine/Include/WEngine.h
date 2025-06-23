#pragma once

#include "WCore/WCore.h"

#include <memory>

class WRender;
class WObjectManager;
class WImportersRegister;
class WImporter;

class WENGINE_API WEngine
{

public:

    WEngine();    

    void run();

    
    // WImporter * GetImporter()

private:

    void InitializeObjectManager();

    void InitializeImporters();

    void InitializeRender();

    std::unique_ptr<WRender> render_{nullptr};

    std::unique_ptr<WObjectManager> object_manager_{nullptr};

    std::unique_ptr<WImportersRegister> importers_register_{nullptr};
    
};

