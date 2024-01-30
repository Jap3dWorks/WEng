#pragma once

#include "WCore.h"
#include <vector>


class WRenderCommand;

class WRENDER_API WRenderPool
{
private:
    std::vector<WRenderCommand*> commands_;
    
};
