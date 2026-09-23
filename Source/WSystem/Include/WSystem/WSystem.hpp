#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TFunction.hpp"

class WEngine;
namespace was { class Level; }

namespace wsm {

    struct SystemParameters {
        WEngine * engine;
        was::Level * level;
    };

    using SystemFn = TFnPtr<bool(wsm::SystemParameters const &)>;

}
