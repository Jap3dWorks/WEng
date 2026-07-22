#pragma once

#include <bitset>

namespace wcr::debug {

#ifndef NDEBUG
    inline constexpr bool kDebug{true};
#else
    inline constexpr bool kDebug{false};
#endif

}

#define WCORE_DEBUG_ONLY(_X) if constexpr(wcr::debug::kDebug) {_X;}

