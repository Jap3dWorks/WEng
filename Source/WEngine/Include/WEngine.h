#pragma once

#include <memory>

#include "WCore/WCore.h"


class WRender;
class WObjectManager;
class WImporterRegister;

class WENGINE_API WEngine
{

public:

    WEngine();    

private:

    std::unique_ptr<WRender> render{nullptr};

    std::unique_ptr<WObjectManager> object_manager{nullptr};

    std::unique_ptr<WImporterRegister> importer_register_{nullptr};

    
};
