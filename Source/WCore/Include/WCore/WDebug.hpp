#pragma once

namespace wcr::debug {

#ifndef NDEBUG
    inline constexpr bool kDebug{true};
#else
    inline constexpr bool kDebug{false};
#endif

}

#define WCORE_DEBUG_ONLY(_X) if constexpr(wcr::debug::kDebug) {X;}


#define WCORE_DEBUG_ONLY_INIT if constexpr(wcr::debug::kDebug) {

#define WCORE_DEBUG_ONLY_END }
