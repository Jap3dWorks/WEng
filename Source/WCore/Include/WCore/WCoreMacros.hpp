#pragma once

#define WNODISCARD [[nodiscard]]

#ifndef NDEBUG
#define WDEBUGBUILD 1
#else
#define WDEBUGBUILD 0
#endif

#ifndef _STR
#define _XSTR(x) #x
#define _STR(x) _XSTR(x)
#endif

#if defined(_MSC_VER)
    #define HOT __forceinline
#elif defined(__GNUC__) || defined(__clang__)
    #define HOT __attribute__((always_inline)) inline
#else
    #define HOT inline // Fallback
#endif

#if !defined(NDEBUG) && !defined(FORCE_INLINE_IN_DEBUG)
    #undef HOT
    #define HOT inline
#endif

#define WNSPCE()

#define WENG_MAX_ASSET_IDS 16
