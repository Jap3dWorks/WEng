#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TFunction.hpp"
#include "WSystem/WSystemMacros.hpp"

class WEngine;
namespace was { class Level; }

// TODO namespace encapsulation

struct WSystemParameters {
    WEngine * engine;
    was::Level * level;
};

using WSystemFn = TFnPtr<bool(const WSystemParameters &)>;

