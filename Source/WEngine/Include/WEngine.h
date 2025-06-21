#pragma once

#include <memory>

#include "WCore/WCore.h"


class WRender;
class WObjectManager;

class WENGINE_API WEngine
{

public:

    

private:

    std::unique_ptr<WRender> render{nullptr};

    std::unique_ptr<WObjectManager> object_manager{nullptr};

    
};
